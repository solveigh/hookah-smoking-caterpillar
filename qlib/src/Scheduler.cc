
#include "Scheduler.h"

namespace qlib {

Define_Module(Scheduler);

void Scheduler::initialize() {

	_nofPriorityClasses = par("nofCoS");
	cout << "Scheduler numberOfPriorityClasses N = " << _nofPriorityClasses << endl;

	const char *algName = par("routingAlgorithm");
	if (strcmp(algName, "PRIO") == 0) {
		_routingAlgorithm = ALG_PRIO;
		_schedulingAlgorithm = "PRIO";
	} else if (strcmp(algName, "RR") == 0) {
		_routingAlgorithm = ALG_RR;
		_schedulingAlgorithm = "RR";
		_rrIndex = _nofPriorityClasses-1;
	} else if (strcmp(algName, "LQF+") == 0) {
		_routingAlgorithm = ALG_LQFP;
		_schedulingAlgorithm = "LQFP";
	} else if (strcmp(algName, "WRQ") == 0) {
		_routingAlgorithm = ALG_WRQ;
		_schedulingAlgorithm = "WRQ";
	} else if (strcmp(algName, "DRR+") == 0) {
		_routingAlgorithm = ALG_DRRP;
		_schedulingAlgorithm = "DRRP";
		_rrIndex = _nofPriorityClasses-2;
	} else if (strcmp(algName, "WFQ+") == 0) {
		_routingAlgorithm = ALG_WFQP;
		_schedulingAlgorithm = "WFQP";
		_rrIndex = _nofPriorityClasses-2;
	}

    // WFQ+
    _wfq_weight[7] = par("wfq_weight7");    // not used by DRR+
    _wfq_weight[6] = par("wfq_weight6");
    _wfq_weight[5] = par("wfq_weight5");
    _wfq_weight[4] = par("wfq_weight4");
    _wfq_weight[3] = par("wfq_weight3");
    _wfq_weight[2] = par("wfq_weight2");
    _wfq_weight[1] = par("wfq_weight1");
    _wfq_weight[0] = par("wfq_weight0");

    // WRQ
    _wrq_weight[7] = par("wrq_weight7");
    _wrq_weight[6] = par("wrq_weight6");
    _wrq_weight[5] = par("wrq_weight5");
    _wrq_weight[4] = par("wrq_weight4");
    _wrq_weight[3] = par("wrq_weight3");
    _wrq_weight[2] = par("wrq_weight2");
    _wrq_weight[1] = par("wrq_weight1");
    _wrq_weight[0] = par("wrq_weight0");

    // DRR+
    _drr_weight[7] = par("drr_weight7");    // not used by DRR+
    _drr_weight[6] = par("drr_weight6");
    _drr_weight[5] = par("drr_weight5");
    _drr_weight[4] = par("drr_weight4");
    _drr_weight[3] = par("drr_weight3");
    _drr_weight[2] = par("drr_weight2");
    _drr_weight[1] = par("drr_weight1");
    _drr_weight[0] = par("drr_weight0");

	cGate* outputgate = gate("pppg");
	_channel = check_and_cast<cDatarateChannel *>(outputgate->getTransmissionChannel());

	_serviceTime = par("serviceTime");
	_startTime = par("startTime");

	// trigger first scheduling operation
	_triggerServiceMsg = new cMessage("triggerServiceMessage");
	scheduleAt(_startTime, _triggerServiceMsg);

	// DRR+
	_bandwidth = par("bandwidth");
	for (int i = 0; i < _nofPriorityClasses; i++) {
		_credit_counter[i] = 0;
		_queue_credit[i] = ceil((double(_drr_weight[i])/100.0 ) * _bandwidth) / 10000;	// adjust configurable weight (in percent) to bandwidth
		    // and reduce time of circulation by dividing with 10000
	}

	// WRQ
	// rand() returns always the same sequence of random numbers
	srand(0);	// seed rand() with the same number to be able to compare the same algorithm in

	// WFQ+
	for (int i = 0; i < _nofPriorityClasses; i++) {
		_wfq_credit[i] = _wfq_weight[i];
		_wfq_counter[i] = 0;
	}

	// Interframe Gap
	_ifg = par("ifg");
	_ifgBytes = 12; //_ifg * 1250*10^9; 1250000000000
} // initialize()

void Scheduler::handleMessage(cMessage *msg) {
	int queueIndex = -1; // by default we drop the packet

	if (msg == _triggerServiceMsg) {
		// a trigger event arrived (either packet transmission finished or simulated cycle event)
		switch (_routingAlgorithm) {

#if 0
		case ALG_FCFS: // K=8, K=3
			FirstComeFirstServed();
			break;
#endif
		case ALG_PRIO: // K=8, K=3
			queueIndex = Priority();
			break;
		case ALG_RR: // K=8, K=3
			queueIndex = RoundRobin();
			break;
		case ALG_DRRP: // K=8, K=3
			queueIndex = DeficitRoundRobinPlus();
			break;
		case ALG_LQFP:	// K=8, K=3
			queueIndex = LongestQueueFirstPlus();
			break;
		case ALG_WRQ:
			queueIndex = WeightedRandomQueuing();
			break;
		case ALG_WFQP:
			queueIndex = WeightedFairQueuingPlus();
			break;
		default:
			queueIndex = -1;
			break;
		} // end switch

		if (queueIndex > -1) {
			// request from queue queueIndex
			IPassiveQueue *q = getQueue(queueIndex);
			if (q != NULL) {
				if (q->length() > 0) {
					q->request(0);
				}
				cancelEvent(_triggerServiceMsg);

				// request finished, schedule next trigger event
				_triggerServiceMsg->setTimestamp();
				scheduleAt(simTime() + _serviceTime, _triggerServiceMsg);
			}
		} else {
			cancelEvent(_triggerServiceMsg);
			scheduleAt(simTime() + _serviceTime, _triggerServiceMsg);
		}
	} else {
		// a requested packet arrived
		WRPacket* p = check_and_cast<WRPacket*>(msg);
		//cout << simTime() << " " << p->getTimestamp() << " " << p->getTotalQueueingTime() << " "<< p->getTotalServiceTime() << endl;
		//cout << "triggerServiceMsg " << triggerServiceMsg->getSendingTime() << " " << simTime()-triggerServiceMsg->getArrivalTime() << " " << triggerServiceMsg->getTimestamp() << " " << triggerServiceMsg->getArrivalTime() << endl;
		//cout << simTime() << " " << p->getArrivalTime() << " " << triggerServiceMsg->getArrivalTime() << " " << triggerServiceMsg->getSendingTime() << endl;
		p->setTotalServiceTime(
				p->getTotalServiceTime()
						+ (_triggerServiceMsg->getArrivalTime()
								- _triggerServiceMsg->getSendingTime()));
		send(p, "pppg");

		// Cancel and re-schedule trigger
		cancelEvent(_triggerServiceMsg);

		// Notify ourselves the moment the transmission line finishes transmitting the packet to choose (schedule) the next one.
		// Consider Interframe gap ifg
		simtime_t ft =
				gate("pppg")->getTransmissionChannel()->getTransmissionFinishTime();
		scheduleAt(ft+_ifg, _triggerServiceMsg);
	}
} // handleMessage()

int Scheduler::RoundRobin() {

#if 1
	// work-conserving Round-Robin
	if (_rrIndex < 0)	// reset
		_rrIndex = _nofPriorityClasses-1;

	int queueIndex = -1;
	for( int i=_rrIndex; i>=0; i-- ) {
		if( getQueue(i)->length()>0 ) {
			queueIndex = i;
			_rrIndex--;	// remember as start-point for next cycle
			break;
		} else {
			_rrIndex--;
		}
	}
	return queueIndex;
#else
	// non work-conserving Round-Robin
	if (_rrIndex < 0)	// reset
		_rrIndex = _nofPriorityClasses-1;

	int queueIndex = _rrIndex;
	_rrIndex--;
	return queueIndex;
#endif
} // RoundRobin()

int Scheduler::Priority() {
	/*maciej lipinski, cern, email 10.01.2012
	1. wait for the current package to be sent (regardless off the priority)
	2. priority = 7
	3. while (priority >= 0) {
	4.      if (outputQueue[priority] is not empty )
	5.          send frame from the queue of priority
	6.      else
	7.          priority-- }*/

	int queueIndex=-1;
	// go through all priority queues until you find a packet,
	// start with highest priority
	for( int index = _nofPriorityClasses-1; index>=0; index-- ) {
		if( getQueue(index)->length()>0 ) {
			queueIndex = index;
		}
		if(queueIndex!=-1)
			break;
	}

	//cout << "priority chosen: " << queueIndex << endl;
	return queueIndex;
} // Priority()

int Scheduler::LongestQueueFirstPlus() {
	int queueIndex = -1;
	// highest priority queue
	if( getQueue(_nofPriorityClasses-1)->length()>0 ) {
		queueIndex = _nofPriorityClasses-1;
	} else {
		int maxQLIndex = 0;
		for( int i=_nofPriorityClasses-2; i>=0; i-- ) {
			// find queue with maximum length
			if( maxQLIndex < getQueue(i)->length() ) {  // treat only non-empty queues
				maxQLIndex = getQueue(i)->length();	// maximum queue length
				queueIndex = i;			// index of queue with maximum length
			}
		}
	}
	return queueIndex;
} // LongestQueueFirstPlus()

int Scheduler::WeightedFairQueuingPlus() {
	int queueIndex = -1;


#if 1
    // work-conserving WFQ+
    if( getQueue(_nofPriorityClasses-1)->length() > 0 ) {    // treat highest priority class separately
        queueIndex = _nofPriorityClasses-1;
    } else {    // treat standard priority queues fairly
        if( _rrIndex<0 )    // reset Round-Robin counter
            _rrIndex = _nofPriorityClasses-2;

        //for( int i=_rrIndex; i>-1; i-- )
        {
            if( getQueue(_rrIndex)->length() > 0 ) {    // consider only non-empty queues
                if (_wfq_counter[_rrIndex] == 0)
                    _wfq_counter[_rrIndex] = _wfq_credit[_rrIndex];    // restore credit counter to allowed ratio of bandwidth

                int queue_length = getQueue(_rrIndex)->length(); // packet size in bytes of oldest packet in queue

                if( queue_length <= _wfq_counter[_rrIndex] ) {  // if queue's credit is available
                    queueIndex = _rrIndex;  // select this queue for next transmission
                    _wfq_counter[_rrIndex] -= 1;   // decrement credit counter of this queue by the next removed packet's size
                } else {
                    // give more credit to a queue if necessary
                    _wfq_counter[_rrIndex] += _wfq_credit[_rrIndex];
                    _rrIndex--; // decrement _rrIndex
                }
            } else {
                _wfq_counter[_rrIndex] = 0;  // reset credit counter
                _rrIndex--; // decrement _rrIndex
            }
        }
    }
#else
	// work-conserving WFQ
	for( int index=_nofPriorityClasses-1; index>-1; index-- ) {
		if( getQueue(index)->length() > 0 ) {	// consider only non-empty queues
			int qlength = getQueue(index)->length();	// queue length

			if( _wfq_credit[index] == 0 ) {	// queue credit was reset
				_wfq_credit[index] = _wfq_weight[index];	// restore credit counter to allowed ratio of bandwidth
			}
			if( _wfq_credit[index]>=1 ) {	// if queue's credit is available
				queueIndex = index;	// select this queue for next transmission
				_wfq_credit[index] -= 1;	// decrement credit counter of this queue by the next removed packet
				break;	// non-empty queue found
			} else {
				// give more credit to a queue if necessary
				_wfq_credit[index] += _wfq_weight[index];
			}
		} else {
			_wfq_credit[index] = 0;	// reset queue credit to zero
		}
	}
#endif
	return queueIndex;
}	// WeightedFairQueuingPlus()

int Scheduler::DeficitRoundRobinPlus() {
	int queueIndex = -1;

#if 1
    // work-conserving DRR+
    if( getQueue(_nofPriorityClasses-1)->length() > 0 ) {    // treat highest priority class separately
        queueIndex = _nofPriorityClasses-1;
    } else {    // treat standard priority queues fairly
        if( _rrIndex<0 )    // reset Round-Robin counter
            _rrIndex = _nofPriorityClasses-2;

        if( getQueue(_rrIndex)->length() > 0 ) {    // consider only non-empty queues
            if (_credit_counter[_rrIndex] == 0)
                _credit_counter[_rrIndex] = _queue_credit[_rrIndex];    // restore credit counter to allowed ratio of bandwidth

            int packet_size = getQueue(_rrIndex)->front()->getByteLength(); // packet size in bytes of oldest packet in queue

            if( (packet_size) <= _credit_counter[_rrIndex] ) {  // if queue's credit is available
                queueIndex = _rrIndex;  // select this queue for next transmission
                _credit_counter[_rrIndex] -= packet_size;   // decrement credit counter of this queue by the next removed packet's size
            } else {
                // give more credit to a queue if necessary
                _credit_counter[_rrIndex] += _queue_credit[_rrIndex];
                _rrIndex--; // decrement _rrIndex
            }
        } else {
            _credit_counter[_rrIndex] = 0;  // reset credit counter
            _rrIndex--; // decrement _rrIndex
        }
    }
#else
    // work-conserving DRR
    if( _rrIndex<0 )    // reset Round-Robin counter
        _rrIndex = _nofPriorityClasses-1;

    if( getQueue(_rrIndex)->length() > 0 ) {    // consider only non-empty queues
        if (_credit_counter[_rrIndex] == 0)
            _credit_counter[_rrIndex] = _queue_credit[_rrIndex];    // restore credit counter to allowed ratio of bandwidth

        int packet_size = getQueue(_rrIndex)->front()->getByteLength(); // packet size in bytes of oldest packet in queue

        //cout << "_rrIndex: " << _rrIndex << " psize " << packet_size << " creditcnt " << credit_counter[_rrIndex] << " " << queue_credit[_rrIndex] << endl;

        if( (packet_size) <= _credit_counter[_rrIndex] ) {  // if queue's credit is available
            queueIndex = _rrIndex;  // select this queue for next transmission
            _credit_counter[_rrIndex] -= packet_size;   // decrement credit counter of this queue by the next removed packet's size

            //cout << " creditcnt " << credit_counter[_rrCounter] << " " << queue_credit[_rrCounter] << endl;
        } else {
            // give more credit to a queue if necessary
            _credit_counter[_rrIndex] += _queue_credit[_rrIndex];
            _rrIndex--; // decrement _rrIndex
        }
    } else {
        // reset credit counter
        _credit_counter[_rrIndex] = 0;
        // decrement rrCounter
        _rrIndex--;
    }
    //cout << "DRR chosen " << queueIndex << endl;
#endif

#if 0
	// work-conserving DRR, serves all priority queues in a round-robin manner once per scheduler run ("round")
	for( int index=_nofPriorityClasses-1; index>-1; index-- ) {
		if( getQueue(index)->length() > 0 ) {	// consider only non-empty queues
			int packet_size = getQueue(index)->front()->getByteLength();	// packet size in bytes of oldest packet in queue

			if( _credit_counter[index] < packet_size ) {	// credit doesn't last
				_credit_counter[index] = _credit_counter[index]+_queue_credit[index];	// restore credit counter to allowed ratio of bandwidth
			}
			if( (packet_size) <= _credit_counter[index] ) {
				// if queue's credit is available
				queueIndex = index;	// select this queue for next transmission
				_credit_counter[index] -= packet_size;	// decrement credit counter of this queue by the next removed packet's size
				break;	// non-empty queue found
			} else {
				// give more credit to a queue if necessary
				_credit_counter[index] += _queue_credit[index];
			}
		}
	}
#else
    // work-conserving Deficit Round-Robin
    /*if (_rrIndex < 0)   // reset
        _rrIndex = _nofPriorityClasses-1;

    for( int i=_rrIndex; i>=0; i-- ) {
        if( getQueue(i)->length()>0 ) {

            if (_credit_counter[i] == 0) {
                _credit_counter[i] = _queue_credit[i];    // restore credit counter to allowed ratio of bandwidth
            }
            int packet_size = getQueue(i)->front()->getByteLength();
            if( packet_size <= _credit_counter[i] ) {  // if queue's credit is available
                queueIndex = i;  // select this queue for next transmission
                _credit_counter[i] -= packet_size;   // decrement credit counter of this queue by the next removed packet's size

                _rrIndex = i-1; // remember as start-point for next cycle
                break;
                //cout << " creditcnt " << credit_counter[_rrCounter] << " " << queue_credit[_rrCounter] << endl;
            } else {
                // give more credit to a queue if necessary for the next round (scheduler run)
                _credit_counter[i] += _queue_credit[i];
                _rrIndex = i-1; // decrement _rrIndex for next round
            }
        } else {
            _credit_counter[i] = 0;
        }
    }

    return queueIndex;
	*/
#endif
	return queueIndex;
} // DeficitRoundRobinPlus()

int Scheduler::WeightedRandomQueuing() {
	int queueIndex = -1;

	// w_i / sum(w_j) must be guaranteed
	// w_i = weight of current priority class
	// w_j = weight of other, non-empty queue

	int sumNonEmptyQueueWeights = 0;
	for( int i=_nofPriorityClasses-1; i>-1; i-- ) {
		if( getQueue(i)->length()>0 ) {
			// determine sum of non-empty queues
			sumNonEmptyQueueWeights += _wrq_weight[i];
		}
	}
	//cout << "sumNonEmptyQueueWeights " << sumNonEmptyQueueWeights << endl;

	if( sumNonEmptyQueueWeights>0 ) {
		// select non-empty queue according to guaranteed weight,
		// e.g. by sorting weights into array and selecting randomly a number from this array
		// the number is the index of the array element that holds the index of the queue to serve next,
		// selected with the appropriate probability, the probability of selecting the highest queue will
		// be proven by statistic over a higher number of packets

		// select index randomly from probabilityArray
		// several scenarios
		int randIndex = 0 + rand() % (sumNonEmptyQueueWeights+1);

		// select first non-empty queue that holds packets and whose weight matches the criterion
		int currentWeight=0;
		for( int i=_nofPriorityClasses-1; i>-1; i-- ) {
			if( getQueue(i)->length()>0 ) {
				// determine sum of weights
				currentWeight += _wrq_weight[i];

				if( randIndex <= currentWeight ) {
					queueIndex = i;
					break;
				}
			}
		}
	}

	return queueIndex;
} // WeightedRandomQueuing()

int Scheduler::FirstComeFirstServed() {
	int queueIndex=-1;
	map< double, int >::iterator sit;
	string qname;
	// check packets in all queues, chose oldest
	_mapPacketAges.clear();
	for(int k=(_nofPriorityClasses-1); k>-1; k--) {
		IPassiveQueue *q = getQueue(k);
		if(q->length()>0){

			// implementation trick to treat all available priority queues as a single queue
			// iterate over all packets in queue
			qname = getQueueName(k);
			cQueue cq = q->getQueue();
			//cout << "1 " << qname << " " << cq.length();
			if( getQueue(2) != NULL ) {
				cQueue::Iterator qit = cQueue::Iterator(cq,false);
				while(!qit.end()) {
					WRPacket *p = (WRPacket*)(cObject*)qit();
					double s = p->getCreationTime().dbl();
					_mapPacketAges.insert( pair< double, int >(s, k) );
					qit++;
				}
			}
		}
	}
	//cout << "# packets " << _mapPacketAges.size() << endl;

	if( _mapPacketAges.size()==1) {
		queueIndex = _mapPacketAges.begin()->second;
	}else if(_mapPacketAges.size()>0) {
		sit = _mapPacketAges.end();
		sit--;
		queueIndex = sit->second;
	} else {
		queueIndex = -1;
	}
	return queueIndex;
	//cout << "FCFS chosen " << queueIndex << endl;
	//cout << "fcfs list size " << _fcfsQueueServeList.size() << " chosen " << queueIndex << endl;
} // FirstComeFirstServed()

int Scheduler::findMaxQLengthIndex( int queuelengths[] ) {
	int queueIndex = -1;
	int maxi = 0;

	// find queue with maximum length
	for(int i = _nofPriorityClasses-2; i >= 0; i--) {
		if( maxi < queuelengths[i] ) {
			maxi = queuelengths[i];	// maximum length
			queueIndex = i;			// index of queue with maximum length
		}
	}
	return queueIndex;
} // findMaxQLengthIndex()

IPassiveQueue *Scheduler::getQueue(int index) {
	std::string queue = "queue";
	char buffer[3];

	sprintf(buffer, "%d", index);
	buffer[2] = '\0';
	queue += buffer;
	cModule *module = getParentModule()->getSubmodule(queue.c_str());
	IPassiveQueue *pqueue = dynamic_cast<IPassiveQueue *>(module);
	//cout << "get queue " << queue << endl;
	return pqueue;
} // getQueue()

string Scheduler::getQueueName(int index) {
	std::string queue = "queue";
	char buffer[3];

	sprintf(buffer, "%d", index);
	buffer[2] = '\0';
	queue += buffer;
	//cout << "get queue " << queue << endl;
	return queue;
} // getQueueName()

}
;
//namespace

