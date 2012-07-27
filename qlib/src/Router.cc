//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Router.h"

namespace qlib {

Define_Module(Router);

void Router::initialize() {
	const char *algName = par("routingAlgorithm");
	if (strcmp(algName, "SQF") == 0) {
		routingAlgorithm = ALG_SQF;
		schedulingAlgorithm = "SQF";
	} else if (strcmp(algName, "LQF") == 0) {
		routingAlgorithm = ALG_LQF;
		schedulingAlgorithm = "LQF";
	} else if (strcmp(algName, "WFQ") == 0) {
		routingAlgorithm = ALG_WFQ;
		schedulingAlgorithm = "WFQ";
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

	if( _nofCoS==3 ) {
		rrCounter = 2;

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
		weight[2] = _weight7 - 3;
		weight[1] = _weight7 - 3;
		weight[0] = _weight7 - 4;

		// priority
		_priorityCounter = 2;

	} else if( _nofCoS==8 ) {
		rrCounter = 7;

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
		weight[7] = _weight7;
		weight[6] = _weight7 - 1;
		weight[5] = _weight7 - 2;
		weight[4] = _weight7 - 2;
		weight[3] = _weight7 - 3;
		weight[2] = _weight7 - 3;
		weight[1] = _weight7 - 3;
		weight[0] = _weight7 - 4;
		
		// priority
		_priorityCounter = 7;

	}	// else N=8

	cGate* outputgate = gate("pppg");
	channel = check_and_cast<cDatarateChannel *>(
			outputgate->getTransmissionChannel());

	serviceTime = par("serviceTime");
	triggerServiceMsg = new cMessage("triggerServiceMessage");
	scheduleAt(serviceTime, triggerServiceMsg); // trigger starting requests

	// wrr
	for (int i = 0; i < _nofCoS; i++) {
		credit_counter[i] = 0;
		queue_credit[i] = 0;
	}

	// WFQ
	for (int i = 0; i < _nofCoS; i++) {
		wfq_counter[i] = 0;
	}

	_ifg = 12;
} // initialize()

void Router::handleMessage(cMessage *msg) {
	vector<IPassiveQueue*>::iterator it;
	vector<IPassiveQueue*> p30;
	map<int, int>::iterator mit;
	map< double, int >::iterator sit;
	int i = _nofCoS - 2;
	int opcnt = -1;
	cQueue *q2;
	string qname;

	int queueIndex = -1; // by default we drop the message
	if (msg == triggerServiceMsg) {
		switch (routingAlgorithm) {
		/*case ALG_RANDOM: // N=8, N=3
			queueIndex = 0 + int(gateSize("in") * rand() / (RAND_MAX + 1.0));
			opcnt = 1;
			//cout << __FILE__ << " " << simTime() << " gatesize in " << gateSize("in") << " randomQIndex " << queueIndex << endl;
			break;
		case ALG_ROUND_ROBIN:  // N=8, N=3
			// default RR
			queueIndex = rrCounter;
			rrCounter = (rrCounter + 1) % gateSize("in");
			opcnt = 1;
			//cout << __FILE__ << " " << simTime() << " gatesize in " << gateSize("in") << " RR " << rrCounter << " " << queueIndex << endl;
			break;*/
		case ALG_WRR: // N=8, N=3
			queue_credit[rrCounter] = 1000000000 * weight[rrCounter] / 1000000;
			if (getQueue(rrCounter)->length() > 0) {
				if (credit_counter[rrCounter] == 0)
					credit_counter[rrCounter] = queue_credit[rrCounter];
				_packet_size = getQueue(rrCounter)->front()->getByteLength();
				//cout << "rrCounter: " << rrCounter << " psize " << packet_size << " creditcnt " << credit_counter[rrCounter] << " " << queue_credit[rrCounter] << endl;
				if ((_packet_size + _ifg) <= credit_counter[rrCounter]) {
					queueIndex = rrCounter;
					credit_counter[rrCounter] -= _packet_size + _ifg;
					//cout << " creditcnt " << credit_counter[rrCounter] << " " << queue_credit[rrCounter] << endl;
					if (credit_counter[rrCounter] == 0) {
						rrCounter = (rrCounter + 1) % _nofCoS;
					}
					break;
				} else {
					// give more credit
					credit_counter[rrCounter] += queue_credit[rrCounter];
					rrCounter = (rrCounter + 1) % _nofCoS;
				}
			} else {
				credit_counter[rrCounter] = 0;
				// goto next queue
				rrCounter = (rrCounter + 1) % _nofCoS;
			}
			//cout << "WRR chosen " << queueIndex << endl;
			break;
		case ALG_SQF:
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
			if( ((_nofCoS==8)&&(_q7->length()==0)) || ((_nofCoS==3)&&(_q2->length()==0)) ) {
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
			if( ((_nofCoS==8)&&(_q7->length()==0)) || ((_nofCoS==3)&&(_q2->length()==0)) ) {
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
		case ALG_WFQ:
		
#if 1
		if( (_nofCoS==8) || (_nofCoS==3) ) {
			// TODO remember which queue was served last???
			for( i=_nofCoS-1; i>-1; i-- ) {
				if( wfq_counter[i] == wfq_weight[i] ) {
					wfq_counter[i] = 0;
				}
				if( wfq_counter[i] < wfq_weight[i] ) {
					if (getQueue(i)->length() > 0) { // try up to wfq_weight[i] times
						queueIndex = i;
					} else {
						queueIndex = -1;
					}
					wfq_counter[i]++;
					if( queueIndex!=-1)
						break;
				}
			}
			//cout << "wfq chosen: " << queueIndex << " wfq_counter[q] " << wfq_counter[queueIndex] << endl;
		}
#else
			if( _nofCoS==8 ) {
				if (_counter7 < _weight7) {
					if (_q7->length() > 0) { // try up to _weight7 times
						queueIndex = _nofCoS-1;
					}
					_counter7++;
					break;
				} else {
					if (_counter6 < (_weight7 - 1)) {
						if (getQueue(_nofCoS-2)->length() > 0) { // try up to _weight7-1 times
							queueIndex = _nofCoS-2;
						}
						_counter6++;
						break;
					} else if (_counter6 == (_weight7 - 1)) {
						// 5..0
						i = _nofCoS - 3;
						//for (it = _qs.begin(); it!=_qs.end(); it++, i--) {
						for (i = _nofCoS - 3; i > -1; i--) {
							if (getQueue(i)->length() > 0) {
								queueIndex = i;
								break;
							}
						}
						_counter6 = 0;
					}
					if (_counter7 == _weight7)
						_counter7 = 0;
				}
			} else if(_nofCoS==3) {
				if (_counter2 < _weight2) {
					if (_q2->length() > 0) { // try up to _weight2 times
						queueIndex = _nofCoS-1;
					}
					_counter2++;
					break;
				} else {
					if (_counter1 < (_weight2 - 1)) {
						if (getQueue(_nofCoS-2)->length() > 0) { // try up to _weight2-1 times
							queueIndex = _nofCoS-2;
						}
						_counter1++;
						break;
					} else if (_counter1 == (_weight2 - 1)) {
						// 0
						if (getQueue(0)->length() > 0) {
							queueIndex = 0;
						}
						_counter1 = 0;
					}
					if (_counter2 == _weight2)
						_counter2 = 0;
				}
			}
#endif
			//cout << "wfq chosen: " << queueIndex << " counter7 " << _counter7 << endl;
			break;
		case ALG_RR: // N=8, N=3
			if (_priorityCounter < 0)
				_priorityCounter = _nofCoS-1;
			queueIndex = _priorityCounter;
			_priorityCounter--;
			//cout << "priority chosen: " << queueIndex << endl;
			break;
		case ALG_FCFS: // N=8, N=3
			//queueIndex = getFromQueueServeList();
			//queueIndex = retrieveFromQueueServeList(); // last approach

			// check packets in all queues, chose oldest
			_mapPacketAges.clear();
			for(int k=(_nofCoS-1); k>-1; k--) {
				IPassiveQueue *q = getQueue(k);
				if(q->length()>0){
					//WRPacket*p = q->front();
					//double s = p->getCreationTime().dbl();

					// iterate over all packets in queue???
					qname = getQueueName(k);
					cQueue cq = q->getQueue();
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

			/*for( sit=_mapPacketAges.begin(); sit !=_mapPacketAges.end(); sit++ ){
				cout << "prio " << sit->second << " " << sit->first << endl;
			}*/
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
		/*case ALG_MIXED:
			if( _nofCoS==8 ) {
				// 7, fcfs
				if (_q7->length() > 0) {
					queueIndex = _nofCoS-1;
					break;
				}
				if (queueIndex == -1) {
					// 6..4, lqf
					p30.clear();
					for (int j = (_nofCoS-2); j > (_nofCoS-5); j--) {
						//cout << "j " << j << " " << _qs.at(j)->length() << " " << getQueue(j)->length() << endl;
						p30.push_back(getQueue(j));
					}

					//determineQueueSizes();
					_mapQSizes.clear();
					i = _nofCoS - 2;
					// request oldest packet first (supposedly in longest queue)
					for (it = p30.begin(); it != p30.end(); it++, i--) {
						if ((*it)->length() > 0) {
							//cout << i << ": " << (*it)->length() << endl;
							_mapQSizes.insert(pair<int, int>((*it)->length(), i));
						}
					}
					if (_mapQSizes.size() > 0) {
						mit = _mapQSizes.end();
						mit--;
						queueIndex = (*mit).second;
						//cout << simTime() << " mixed chosen: " << queueIndex << endl;
					} else {
						queueIndex = -1;
					}
				}
				if (queueIndex == -1) {
					// sqf, 3..0
					p30.clear();
					for (int j = 3; j > -1; j--) {
						//cout << "j " << j << " " << _qs.at(j)->length() << " " << getQueue(j)->length() << endl;
						p30.push_back(getQueue(j));
					}

					_mapQSizes.clear();
					i = _nofCoS - 5;
					// request oldest packet first (supposedly in longest queue)
					for (it = p30.begin(); it != p30.end(); it++, i--) {
						if ((*it)->length() > 0) {
							//cout << i << ": " << (*it)->length() << endl;
							_mapQSizes.insert(pair<int, int>((*it)->length(), i));
						}
					}
					if (_mapQSizes.size() > 0) {
						mit = _mapQSizes.begin();
						queueIndex = (*mit).second;
						//cout << simTime() << " mixed chosen: " << queueIndex << endl;
					} else {
						queueIndex = -1;
					}
				}
			} else if(_nofCoS==3) {
				EV << "This algorithm doesn't make sense for N=3.\n";
				cout << "This algorithm doesn't make sense for N=3." << endl;
				exit(0);
			}
			//cout << simTime() << " mixed chosen: " << queueIndex << endl;
			break;*/
		case ALG_FQSW: // N=8, N=3
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
				//map<int,int>::iterator mit;
				for (mit = _mapQSizes.begin(); mit != _mapQSizes.end(); mit++) {
					//cout << "prio: " << mit->first << " length " << mit->second << endl;
					queueIndex = determineQIndex(mit, mit->first); // TODO determine highest
					_highestIndex.insert(queueIndex);

				}
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

				// TODO? schedule imagined finish of request? Is datarate relevant between queues and router?
				triggerServiceMsg->setTimestamp();
				scheduleAt(simTime() + serviceTime, triggerServiceMsg);
			}
		} else {
			cancelEvent(triggerServiceMsg);
			scheduleAt(simTime() + serviceTime, triggerServiceMsg);
		}
	} else {
		WRPacket* p = check_and_cast<WRPacket*>(msg);
		//cout << simTime() << " " << p->getTimestamp() << " " << p->getTotalQueueingTime() << " "<< p->getTotalServiceTime() << endl;
		//cout << "triggerServiceMsg " << triggerServiceMsg->getSendingTime() << " " << simTime()-triggerServiceMsg->getArrivalTime() << " " << triggerServiceMsg->getTimestamp() << " " << triggerServiceMsg->getArrivalTime() << endl;
		//cout << simTime() << " " << p->getArrivalTime() << " " << triggerServiceMsg->getArrivalTime() << " " << triggerServiceMsg->getSendingTime() << endl;
		p->setTotalServiceTime(
				p->getTotalServiceTime()
						+ (triggerServiceMsg->getArrivalTime()
								- triggerServiceMsg->getSendingTime()));

		send(p, "pppg");

		opcnt = determinOperationCount(routingAlgorithm);
		p->setOperationCounter(p->getOperationCounter() + opcnt);

		// cancel and re-schedule
		cancelEvent(triggerServiceMsg);

		// Notify ourselves the moment the transmission line finishes transmitting the packet.
		simtime_t ft =
				gate("pppg")->getTransmissionChannel()->getTransmissionFinishTime();
		scheduleAt(ft, triggerServiceMsg);
	}
} // handleMessage()

int Router::determineQIndex(map<int, int>::iterator mit, int priority) {
	int queueIndex = -1;
	weight[priority] = (mit->first + 1) * (mit->second); // ensure that priority 0 will be considered as well
	queue_credit[priority] = (1000 * weight[priority]);
	//cout << " queue_credit["<<priority<<"] " << queue_credit[priority] << " weight["<<priority<<"] " << weight[priority] << " credit_cnt " << credit_counter[priority] << endl;
	if (credit_counter[priority] == 0)
		credit_counter[priority] = queue_credit[priority];
	_packet_size = getQueue(priority)->front()->getByteLength();
	credit_counter[priority] -= _packet_size + _ifg;
	if ((_packet_size + _ifg) <= credit_counter[priority]) {
		queueIndex = priority;
		credit_counter[priority] -= _packet_size + _ifg;
	} else {
		// give more credit
		credit_counter[priority] += queue_credit[priority];
	}
	return queueIndex;
} // determineQIndex()

int Router::determinOperationCount(int routingAlgorithm) {
	int op = -1;

	switch (routingAlgorithm) {
	case ALG_FCFS:
		op = 1;
		break;
	case ALG_RR:
		op = _nofCoS;
		break;
	case ALG_SQF:
		op = 1;
		break;
	case ALG_LQF:
		op = 1;
		break;
	case ALG_WFQ:
		op = _nofCoS - 1;
		break;
	case ALG_FQSW:
		op = 1;
		break;
	default:
		break;
	}

	return op;
} // determinOperationCount()

IPassiveQueue *Router::getQueue(int index) {
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

string Router::getQueueName(int index) {
	std::string queue = "queue";
	char buffer[3];

	sprintf(buffer, "%d", index);
	buffer[2] = '\0';
	queue += buffer;
	//cout << "get queue " << queue << endl;
	return queue;
} // getQueueName()

void Router::determineQueueSizes() {

	for (int i = 0; i < (_nofCoS + 1); i++) {
		IPassiveQueue *pqueue = getQueue(i);
		if (pqueue != NULL)
			std::cout << "queue " << i << " length " << pqueue->length()
					<< std::endl;
	}
} // determineQueueSizes()

int Router::calculateMaxWeight(int weight[], int asize) {
	int maxval = 0, i = 0;

	for (i = 0; i < asize; i++) {
		if (i == 0)
			maxval = weight[i];

		else if (weight[i] > maxval)
			maxval = weight[i];
	}
	return maxval;
} // calculateMaxWeight()

int Router::calculateHighestCommonDivisor(int weight[], int asize) {
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

int Router::sumWeights(int weight[], int size) {
	int sum = 0;
	for (int i = 0; i < size; i++)
		sum += weight[i];
	return sum;
} // sumWeights()

}
;
//namespace

