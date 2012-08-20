
#include "Scheduler.h"

namespace qlib {

Define_Module(Scheduler);

void Scheduler::initialize() {
	const char *algName = par("routingAlgorithm");
	if (strcmp(algName, "SQF") == 0) {
		routingAlgorithm = ALG_SQF;
		schedulingAlgorithm = "SQF";
	} else if (strcmp(algName, "LQF") == 0) {
		routingAlgorithm = ALG_LQF;
		schedulingAlgorithm = "LQF";
	} else if (strcmp(algName, "WFQ_RR") == 0) {
		routingAlgorithm = ALG_WFQ_RR;
		schedulingAlgorithm = "WFQ_RR";
	} else if (strcmp(algName, "WFQ_HP") == 0) {
		routingAlgorithm = ALG_WFQ_HP;
		schedulingAlgorithm = "WFQ_HP";
	} else if (strcmp(algName, "RR") == 0) {
		routingAlgorithm = ALG_RR;
		schedulingAlgorithm = "RR";
	} else if (strcmp(algName, "FCFS") == 0) {
		routingAlgorithm = ALG_FCFS;
		schedulingAlgorithm = "FCFS";
	} else if (strcmp(algName, "WRR") == 0) {
		routingAlgorithm = ALG_WRR;
		schedulingAlgorithm = "WRR";
	} else if (strcmp(algName, "FQSW") == 0) {
		routingAlgorithm = ALG_FQSW;
		schedulingAlgorithm = "FQSW";
	}

	_nofCoS = par("nofCoS");
	cout << "Router nofCoS " << _nofCoS << endl;

	if( _nofCoS==8 ) {
			_rrCounter = 7;

			// keep a pointer to queue 7
			_q7 = getQueue(7);

			// pointers to other queues (6..0)
			for (int i = _nofCoS - 2; i > -1; i--)
				_qs.push_back(getQueue(i));

			// WFQ
			_weight7 = par("weight7");
			_weight6 = par("weight6");
			_weight5 = par("weight5");
			_weight4 = par("weight4");
			_weight3 = par("weight3");
			_weight2 = par("weight2");
			_weight1 = par("weight1");
			_weight0 = par("weight0");
			_counter7 = 0;

			wfq_weight[7] = _weight7;
			wfq_weight[6] = _weight6;
			wfq_weight[5] = _weight5;
			wfq_weight[4] = _weight4;
			wfq_weight[3] = _weight3;
			wfq_weight[2] = _weight2;
			wfq_weight[1] = _weight1;
			wfq_weight[0] = _weight0;

			// WRR
#if 1
			weight[7] = par("wrr_weight7");
			weight[6] = par("wrr_weight6");
			weight[5] = par("wrr_weight5");
			weight[4] = par("wrr_weight4");
			weight[3] = par("wrr_weight3");
			weight[2] = par("wrr_weight2");
			weight[1] = par("wrr_weight1");
			weight[0] = par("wrr_weight0");

#else
			weight[7] = _weight7;	// TODO lower weights may not get 0!!!
			weight[6] = _weight7 - 1;
			weight[5] = _weight7 - 2;
			weight[4] = _weight7 - 2;
			weight[3] = _weight7 - 3;
			weight[2] = _weight7 - 3;
			weight[1] = _weight7 - 3;
			weight[0] = _weight7 - 4;
#endif

	} else if( _nofCoS==3 ) {
		_rrCounter = 2;

		// keep a pointer to queue 2
		_q2 = getQueue(2);

		// pointers to other queues (1..0)
		_qs.push_back(getQueue(1));
		_qs.push_back(getQueue(0));

		// WFQ
		_weight2 = par("weight2");
		_weight1 = par("weight1");
		_weight0 = par("weight0");
		_counter7 = 0;

		wfq_weight[2] = _weight2;
		wfq_weight[1] = _weight1;
		wfq_weight[0] = _weight0;

		// WRR
		_weight7 = par("weight7");
		weight[2] = _weight7 - 3;
		weight[1] = _weight7 - 3;
		weight[0] = _weight7 - 4;
	} // if( _nofCoS==3 )

	cGate* outputgate = gate("pppg");
	channel = check_and_cast<cDatarateChannel *>(
			outputgate->getTransmissionChannel());

	serviceTime = par("serviceTime");
	startTime = par("startTime");

	// trigger first scheduling operation
	triggerServiceMsg = new cMessage("triggerServiceMessage");
	scheduleAt(startTime, triggerServiceMsg);

	// WRR
	for (int i = 0; i < _nofCoS; i++) {
		credit_counter[i] = 0;
		queue_credit[i] = 0;
	}

	// WFQ
	for (int i = 0; i < _nofCoS; i++) {
		wfq_counter[i] = 0;
	}

	_ifg = par("ifg");
	_ifgBytes = 12; //_ifg * 1250*10^9; 1250000000000
} // initialize()

void Scheduler::handleMessage(cMessage *msg) {
	vector<IPassiveQueue*>::iterator it;
	vector<IPassiveQueue*> p30;
	map<int, int>::iterator mit;
	map< double, int >::iterator sit;
	int i = _nofCoS - 2;
	int opcnt = -1;
	cQueue *q2;
	string qname;

	int queueIndex = -1; // by default we drop the packet

	if (msg == triggerServiceMsg) {
		// a trigger event arrived (either packet transmission finished or simulated cycle event)
		switch (routingAlgorithm) {

		case ALG_FCFS: // K=8, K=3
			// check packets in all queues, chose oldest
			_mapPacketAges.clear();
			for(int k=(_nofCoS-1); k>-1; k--) {
				IPassiveQueue *q = getQueue(k);
				if(q->length()>0){

					// implementation trick to treat all available priority queues as a single queue
					// iterate over all packets in queue
					qname = getQueueName(k);
					cQueue cq = q->getQueue();
					//cout << "1 " << qname << " " << cq.length();
					if( q2 != NULL ) {
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

			//cout << "FCFS chosen " << queueIndex << endl;
			//cout << "fcfs list size " << _fcfsQueueServeList.size() << " chosen " << queueIndex << endl;
			break;

		case ALG_RR: // K=8, K=3
#if 1
			if (_rrCounter < 0)	// reset
				_rrCounter = _nofCoS-1;
			queueIndex = _rrCounter;
			_rrCounter--;
#else
			/*maciej lipinski, cern, email 10.01.2012
			1. wait for the current package to be sent (regardless off the priority)
			2. priority = 7
			3. while (priority >= 0) {
			4.      if (outputQueue[priority] is not empty )
			5.          send frame from the queue of priority
			6.      else
			7.          priority-- }*/


			// FIXME RR should be better if the queue length is considered during selection
			//if (_priorityCounter < 0)
				//_priorityCounter = _nofCoS-1;
			//if( _priorityCounter<0 )	// reset
			//	_priorityCounter = _nofCoS-1;

			_priorityCounter = _nofCoS-1;
			// go through queues until you find a packet
			while(_priorityCounter>0) {
				if( getQueue(_priorityCounter)->length()>0 ) {
					queueIndex=_priorityCounter;
					break;
				} else {
					_priorityCounter--;
				}
				if (_priorityCounter < 0) {	// reset + break
					_priorityCounter = _nofCoS-1;
					break;
				}
			}
			//queueIndex = _priorityCounter;
#endif
			//cout << "priority chosen: " << queueIndex << endl;
			break;

		case ALG_WRR: // K=8, K=3
			// adjust priority queues credit to bandwidth (1 Gbps)
			queue_credit[_rrCounter] = 1000000000 * weight[_rrCounter] / 1000000;

			// consider priority queue only if it stores packets
			if (getQueue(_rrCounter)->length() > 0) {
				if (credit_counter[_rrCounter] == 0)
					credit_counter[_rrCounter] = queue_credit[_rrCounter];
				int packet_size = getQueue(_rrCounter)->front()->getByteLength();
				//cout << "_rrCounter: " << _rrCounter << " psize " << packet_size << " creditcnt " << credit_counter[_rrCounter] << " " << queue_credit[_rrCounter] << endl;
				if ((packet_size + _ifg) <= credit_counter[_rrCounter]) {
					// if queue's credit is still available
					queueIndex = _rrCounter;
					credit_counter[_rrCounter] -= packet_size + _ifgBytes;
					//cout << " creditcnt " << credit_counter[_rrCounter] << " " << queue_credit[_rrCounter] << endl;
					if (credit_counter[_rrCounter] == 0) {
						_rrCounter = (_rrCounter + 1) % _nofCoS;
					}
					break;
				} else {
					// give more credit to a queue if needed
					credit_counter[_rrCounter] += queue_credit[_rrCounter];
					_rrCounter = (_rrCounter + 1) % _nofCoS;
				}
			} else {
				// reset credit counter
				credit_counter[_rrCounter] = 0;
				// goto next priority queue
				_rrCounter = (_rrCounter + 1) % _nofCoS;
			}
			//cout << "WRR chosen " << queueIndex << endl;
			break;
		case ALG_SQF:
			if( _nofCoS==8 ) {
				// highest priority is 7
				if (_q7->length() > 0) {
					queueIndex = 7;
				}
			} else if(_nofCoS==3) {
				// highest priority is 2
				if (_q2->length() > 0) {
					queueIndex = 2;
				}
			}
			// sort the other priority queues only if the highest priority queues are empty
			if( (queueIndex==-1) && (((_nofCoS==8)&&(_q7->length()==0)) || ((_nofCoS==3)&&(_q2->length()==0))) ) {
				_mapQSizes.clear();
				// sort queues in ascending order according to their length
				for (it = _qs.begin(); it != _qs.end(); it++, i--) {
					if ((*it)->length() > 0) {
						_mapQSizes.insert(pair<int, int>((*it)->length(), i));
					}
				}
				if (_mapQSizes.size() > 0) {
					queueIndex = _mapQSizes.begin()->second; // shortest queue will be in the beginning
				} else
					queueIndex = -1;
			}
			//cout << "minQLength chosen: " << queueIndex << endl;
			break;
		case ALG_LQF:
			if( _nofCoS==8 ) {
				// highest priority
				if (_q7->length() > 0) {
					queueIndex = 7;
				}
			} else if(_nofCoS==3) {
				// highest priority
				if (_q2->length() > 0) {
					queueIndex = 2;
				}
			}
			// sort the other priority queues only if the highest priority queues are empty
			if( (queueIndex==-1) && ( ((_nofCoS==8)&&(_q7->length()==0)) || ((_nofCoS==3)&&(_q2->length()==0))) ) {
				_mapQSizes.clear();
				// sort queues in ascending order according to their length
				for (it = _qs.begin(); it != _qs.end(); it++, i--) {
					if ((*it)->length() > 0) {
						_mapQSizes.insert(pair<int, int>((*it)->length(), i));
					}
				}
				if (_mapQSizes.size() > 0) {
					mit = _mapQSizes.end(); // longest queue will be in the end
					mit--;
					queueIndex = (*mit).second;
				} else {
					queueIndex = -1;
				}
			}
			//cout << "maxQLength chosen: " << queueIndex << endl;
			break;
		case ALG_WFQ_RR:

			// remember which priority queue was chosen last ->RR manner (closer to literature, kurose09)
			if (_rrCounter < 0)	// reset
				_rrCounter = _nofCoS-1;

			if( wfq_counter[_rrCounter] == wfq_weight[_rrCounter] ) {	// reset queues counter
				wfq_counter[_rrCounter] = 0;
			}
			if( wfq_counter[_rrCounter] < wfq_weight[_rrCounter] ) {
				if (getQueue(_rrCounter)->length() > 0) { // try up to wfq_weight[i] times
					queueIndex = _rrCounter;
					_rrCounter = queueIndex;
					wfq_counter[_rrCounter]++;
				} else {
					queueIndex = -1;
					wfq_counter[_rrCounter]++;
					_rrCounter--;
				}
				//wfq_counter[_rrCounter]++;
				if( queueIndex!=-1)
					break;
			}
			//cout << "wfq chosen: " << queueIndex << " wfq_counter[q] " << wfq_counter[queueIndex] << endl;
			//cout << "wfq chosen: " << queueIndex << " counter7 " << _counter7 << endl;
			break;
		case ALG_WFQ_HP:

			// don't remember the last queue chosen since queues states may have changed,
			// start again from highest priority (no RR)
			for( i=_nofCoS-1; i>-1; i-- ) {
				if( wfq_counter[i] == wfq_weight[i] ) {
					wfq_counter[i] = 0;
				}
				if( wfq_counter[i] < wfq_weight[i] ) {
					if (getQueue(i)->length() > 0) { // try up to wfq_weight[i] times
						queueIndex = i;
						wfq_counter[i]++;
					} else {
						queueIndex = -1;
						wfq_counter[i]++;
					}
					
					if( queueIndex!=-1)
						break;
				}
			}
			//cout << "wfq chosen: " << queueIndex << " wfq_counter[q] " << wfq_counter[queueIndex] << endl;
			//cout << "wfq chosen: " << queueIndex << " counter7 " << _counter7 << endl;
			break;

		case ALG_FQSW: // K=8, K=3
			// consider only queues with contents
			// assign weights according to priority and fullness
			// maybe similar to DRR, credits according to bandwidth
			// watch queue capacity, when getting too full assign higher weight

			_mapQSizes.clear();
			_highestIndex.clear();
			// sort queues in ascending order according to their length
			// shortest queue will be in the beginning, longest queue will be in the end
			for (i = (_nofCoS-1); i > -1; i--) {
				if (getQueue(i)->length() > 0) {
					_mapQSizes.insert(pair<int, int>(i, getQueue(i)->length()));
				}
			}
			if (_mapQSizes.size() > 0) {
				// determine weights according to prio and length
				for (mit = _mapQSizes.begin(); mit != _mapQSizes.end(); mit++) {
					//cout << "prio: " << mit->first << " length " << mit->second << endl;
					queueIndex = determineQIndex(mit, mit->first); // determine queue weights dynamically
					_highestIndex.insert(queueIndex);

				}
				// choose the queue with the highest precedence
				if (_highestIndex.size() > 0) {
					set<int>::iterator sit = _highestIndex.end();
					sit--;
					queueIndex = (*sit);
					//cout << "queueIndex " << queueIndex << endl;
				}
			} else {
				queueIndex = -1;
			}
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
				cancelEvent(triggerServiceMsg);

				// request finished, schedule next trigger event
				triggerServiceMsg->setTimestamp();
				scheduleAt(simTime() + serviceTime, triggerServiceMsg);
			}
		} else {
			cancelEvent(triggerServiceMsg);
			scheduleAt(simTime() + serviceTime, triggerServiceMsg);
		}
	} else {
		// a requested packet arrived
		WRPacket* p = check_and_cast<WRPacket*>(msg);
		//cout << simTime() << " " << p->getTimestamp() << " " << p->getTotalQueueingTime() << " "<< p->getTotalServiceTime() << endl;
		//cout << "triggerServiceMsg " << triggerServiceMsg->getSendingTime() << " " << simTime()-triggerServiceMsg->getArrivalTime() << " " << triggerServiceMsg->getTimestamp() << " " << triggerServiceMsg->getArrivalTime() << endl;
		//cout << simTime() << " " << p->getArrivalTime() << " " << triggerServiceMsg->getArrivalTime() << " " << triggerServiceMsg->getSendingTime() << endl;
		p->setTotalServiceTime(
				p->getTotalServiceTime()
						+ (triggerServiceMsg->getArrivalTime()
								- triggerServiceMsg->getSendingTime()));
		send(p, "pppg");

		// cancel and re-schedule
		cancelEvent(triggerServiceMsg);

		// Notify ourselves the moment the transmission line finishes transmitting the packet to choose (schedule) the next one.
		simtime_t ft =
				gate("pppg")->getTransmissionChannel()->getTransmissionFinishTime();
		scheduleAt(ft+_ifg, triggerServiceMsg);
	}
} // handleMessage()

int Scheduler::determineQIndex(map<int, int>::iterator mit, int priority) {
	int queueIndex = -1;

	// calculate weight dynamically by multiplying priority and queue length
	// +1: ensure that priority 0 will be considered as well

	// assign a minimum weight to each lower priority queue	
	int priority_weight = (mit->first + 1);

#if 0
	// allow a minimum to prevent high queuing times for K=8
	if( (_nofCoS==8) && (priority>3) )
		priority_weight = 2;
#endif

	weight[priority] = priority_weight * (mit->second);

	// adjust to bandwidth of 1 Gbps
	queue_credit[priority] = (1000 * weight[priority]);	// re-assign queue credit (is this a good idea to keep the old values? in the next cycle queue lengths will have changed)
	//cout << " queue_credit["<<priority<<"] " << queue_credit[priority] << " weight["<<priority<<"] " << weight[priority] << " credit_cnt " << credit_counter[priority] << endl;

	if (credit_counter[priority] == 0)
		credit_counter[priority] = queue_credit[priority];
	int packet_size = getQueue(priority)->front()->getByteLength();
	credit_counter[priority] -= packet_size + _ifgBytes;

	// select queue according to available queue credit
	if ((packet_size + _ifg) <= credit_counter[priority]) {
		queueIndex = priority;
		credit_counter[priority] -= packet_size + _ifgBytes;
	} else {
		// give more credit
		credit_counter[priority] += queue_credit[priority];
		queueIndex = -1;
	}
	return queueIndex;
} // determineQIndex()

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

void Scheduler::determineQueueSizes() {

	for (int i = 0; i < (_nofCoS + 1); i++) {
		IPassiveQueue *pqueue = getQueue(i);
		if (pqueue != NULL)
			std::cout << "queue " << i << " length " << pqueue->length()
					<< std::endl;
	}
} // determineQueueSizes()

int Scheduler::calculateMaxWeight(int weight[], int asize) {
	int maxval = 0, i = 0;

	for (i = 0; i < asize; i++) {
		if (i == 0)
			maxval = weight[i];

		else if (weight[i] > maxval)
			maxval = weight[i];
	}
	return maxval;
} // calculateMaxWeight()

int Scheduler::calculateHighestCommonDivisor(int weight[], int asize) {
	int i, a, t, b;
	a = weight[0];

	// However, this means that all except the first weight value gets reset to 0.
	// All you need to do is add in a working variable b, assign it from weight[i+1],
	// and use b in the GCD loop.
	for (i = 0; i < (asize - 1); i++) {
		/* function gcd(a, b) */
		t = 0;
		b = weight[i + 1];
		while (b != 0) {
			t = b;
			b = a % b;
			a = t;
		}
	}
	return a;
} // calculateHighestCommonDivisor()

int Scheduler::sumWeights(int weight[], int size) {
	int sum = 0;
	for (int i = 0; i < size; i++)
		sum += weight[i];
	return sum;
} // sumWeights()

}
;
//namespace

