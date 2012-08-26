
#include "Scheduler.h"

namespace qlib {

Define_Module(Scheduler);

void Scheduler::initialize() {
	const char *algName = par("routingAlgorithm");
	if (strcmp(algName, "PRIO") == 0) {
		_routingAlgorithm = ALG_PRIO;
		_schedulingAlgorithm = "PRIO";
	} else if (strcmp(algName, "RR") == 0) {
		_routingAlgorithm = ALG_RR;
		_schedulingAlgorithm = "RR";
	} else if (strcmp(algName, "LQF+") == 0) {
		_routingAlgorithm = ALG_LQFP;
		_schedulingAlgorithm = "LQFP";
	} else if (strcmp(algName, "WFQ_RR") == 0) {
		_routingAlgorithm = ALG_WFQ_RR;
		_schedulingAlgorithm = "WFQ_RR";
	} else if (strcmp(algName, "WFQ_HP") == 0) {
		_routingAlgorithm = ALG_WFQ_HP;
		_schedulingAlgorithm = "WFQ_HP";
	} else if (strcmp(algName, "WRR") == 0) {
		_routingAlgorithm = ALG_WRR;
		_schedulingAlgorithm = "WRR";
	} else if (strcmp(algName, "FQSW") == 0) {
		_routingAlgorithm = ALG_FQSW;
		_schedulingAlgorithm = "FQSW";
	}

	_nofCoS = par("nofCoS");
	cout << "Scheduler nofCoS " << _nofCoS << endl;

	if( _nofCoS==8 ) {
		_rrCounter = _nofCoS-1;

		// WFQ
		_wfq_weight[7] = par("wfq_weight7");
		_wfq_weight[6] = par("wfq_weight6");
		_wfq_weight[5] = par("wfq_weight5");
		_wfq_weight[4] = par("wfq_weight4");
		_wfq_weight[3] = par("wfq_weight3");
		_wfq_weight[2] = par("wfq_weight2");
		_wfq_weight[1] = par("wfq_weight1");
		_wfq_weight[0] = par("wfq_weight0");

		// WRR
		_weight[7] = par("wrr_weight7");
		_weight[6] = par("wrr_weight6");
		_weight[5] = par("wrr_weight5");
		_weight[4] = par("wrr_weight4");
		_weight[3] = par("wrr_weight3");
		_weight[2] = par("wrr_weight2");
		_weight[1] = par("wrr_weight1");
		_weight[0] = par("wrr_weight0");

	} else if( _nofCoS==3 ) {
		_rrCounter = _nofCoS-1;

		// WFQ
		_wfq_weight[2] = par("wfq_weight2");
		_wfq_weight[1] = par("wfq_weight1");
		_wfq_weight[0] = par("wfq_weight0");

		// WRR
		_weight[2] = par("wrr_weight2");
		_weight[1] = par("wrr_weight1");
		_weight[0] = par("wrr_weight0");
	} // if( _nofCoS==3 )

	cGate* outputgate = gate("pppg");
	_channel = check_and_cast<cDatarateChannel *>(
			outputgate->getTransmissionChannel());

	_serviceTime = par("serviceTime");
	_startTime = par("startTime");

	// trigger first scheduling operation
	_triggerServiceMsg = new cMessage("triggerServiceMessage");
	scheduleAt(_startTime, _triggerServiceMsg);

	// WRR
	for (int i = 0; i < _nofCoS; i++) {
		_credit_counter[i] = 0;
		_queue_credit[i] = 1000 * _weight[i];
	}

	// WFQ
	for (int i = 0; i < _nofCoS; i++) {
		_wfq_counter[i] = 0;
	}

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
		case ALG_WRR: // K=8, K=3
			queueIndex = WeightedRoundRobin();
			break;
		case ALG_LQFP:	// K=8, K=3
			queueIndex = LongestQueueFirstPlus();
			break;
		case ALG_WFQ_RR:
			queueIndex = WeightedFairQueuingRR();
			break;
		case ALG_WFQ_HP:
			queueIndex = WeightedFairQueuingHP();
			break;
		case ALG_FQSW: // K=8, K=3
			queueIndex = FairQueueSizebasedWeighting();
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

		// cancel and re-schedule
		cancelEvent(_triggerServiceMsg);

		// Notify ourselves the moment the transmission line finishes transmitting the packet to choose (schedule) the next one.
		simtime_t ft =
				gate("pppg")->getTransmissionChannel()->getTransmissionFinishTime();
		scheduleAt(ft+_ifg, _triggerServiceMsg);
	}
} // handleMessage()

int Scheduler::RoundRobin() {
	if (_rrCounter < 0)	// reset
		_rrCounter = _nofCoS-1;

	int queueIndex = _rrCounter;
	_rrCounter--;
	return queueIndex;
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
	// go through all priority queues until you find a packet
	for( int index = _nofCoS-1; index>0; index-- ) {
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
	if( getQueue(_nofCoS-1)->length()>0 ) {
		queueIndex = _nofCoS-1;
	} else {
#if 1
		// queues 6..0
		int queuelengths[_nofCoS-1];

		for( int i=_nofCoS-2; i>=0; i-- ) {
			queuelengths[i] = getQueue(i)->length();
			//cout << "q " << i << " " << queuelengths[i] << endl;
		}

		// find queue with maximum length
		queueIndex = findMaxQLengthIndex(queuelengths);
	}

#else
		// sort the other priority queues only if the highest priority queues are empty
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
#endif
	return queueIndex;
} // LongestQueueFirstPlus()

int Scheduler::WeightedRoundRobin() {
	int queueIndex = -1;
	// consider priority queue only if it stores packets

	/*if( _rrCounter==0 ) {	// reset Round-Robin counter
		_rrCounter = _nofCoS-1;
	}*/

	if (getQueue(_rrCounter)->length() > 0) {
		if (_credit_counter[_rrCounter] == 0)
			_credit_counter[_rrCounter] = _queue_credit[_rrCounter];	// reset credit counter to allowed ratio

		int packet_size = getQueue(_rrCounter)->front()->getByteLength();	// packet size in bytes of oldest packet in queue

		//cout << "_rrCounter: " << _rrCounter << " psize " << packet_size << " creditcnt " << credit_counter[_rrCounter] << " " << queue_credit[_rrCounter] << endl;

		if( (packet_size + _ifg) <= _credit_counter[_rrCounter] ) {	// if queue's credit is available
			queueIndex = _rrCounter;	// select this queue for next transmission
			_credit_counter[_rrCounter] -= packet_size + _ifgBytes;	// decrement credit counter of this queue by the next removed packet's size

			//cout << " creditcnt " << credit_counter[_rrCounter] << " " << queue_credit[_rrCounter] << endl;

			if (_credit_counter[_rrCounter] == 0) {			// queue's allowed credit is reached, decrement rrCounter
				_rrCounter = (_rrCounter + 1) % _nofCoS;
				//_rrCounter--;
				//_rrCounter = (_rrCounter -1) % _nofCoS;
			}
			return queueIndex;
		} else {
			// give more credit to a queue if necessary
			_credit_counter[_rrCounter] += _queue_credit[_rrCounter];
			_rrCounter = (_rrCounter + 1) % _nofCoS;
			//_rrCounter--;
			//_rrCounter = (_rrCounter -1) % _nofCoS;
		}
	} else {
		// reset credit counter
		_credit_counter[_rrCounter] = 0;
		// goto next priority queue
		_rrCounter = (_rrCounter + 1) % _nofCoS;
		//_rrCounter--;
		//_rrCounter = (_rrCounter -1) % _nofCoS;
	}
	//cout << "WRR chosen " << queueIndex << endl;

	return queueIndex;
} // WeightedRoundRobin()

int Scheduler::WeightedFairQueuingRR() {
	int queueIndex = -1;

#if 1
	if (_rrCounter < 0)	// reset
		_rrCounter = _nofCoS-1;

	// consider RR nature  -> leads to similar packet loss in highest priority class as RR!
	int weights[_nofCoS];	// weights in percent

	// initialize weights array
	for( int i=_nofCoS-1; i>=0; i-- ) {
		weights[i] = 0;
	}

	double remainder= 100.0;	// remainder of 100% available bandwidth

	/*int nonEmptyQueues=0;	// for alternative implementation
	for( int i=_nofCoS-1; i>=0; i-- ) {
		if( getQueue(i)->length()>0 ) {
			nonEmptyQueues++;
		}
	}
	cout << "nonEmptyQueues: " << nonEmptyQueues << endl;*/

	for( int i=_nofCoS-1; i>=0; i-- ) {
		if( getQueue(i)->length()>0 ) {
			// distribute weights equally among non-empty priority queues (100%)	-> bad results
			//weights[i] = ceil(100.0/double(nonEmptyQueues));

			// calculate actual weights used based on non-empty priority queues and maximum share of bandwidth
			//weights[i] = ceil( ((double(_wfq_weight[i]))/100.0)*usedUp );	// distribute weights according to max allowed percentage of bandwidth
			//usedUp -= weights[i];

			weights[i] = ceil((remainder / 100.0 ) * (double(_wfq_weight[i])));
			remainder = remainder - weights[i];
			//cout << "rrCounter: " << _rrCounter << " i: " << i << ": confWeight " << _wfq_weight[i] << ", calcWeight " << weights[i] << " remainder: " << remainder << endl;
		}
	}

	// find index of maximum weight in weights array

	int index = 0;
	int maxi = 0;

	// find queue with maximum length, build list of indices to priority queues, correlate to _rrCounter
	for(int i = _nofCoS-1; i >=0 ; i--) {
		if( maxi < weights[i] ) {
			maxi = weights[i];	// maximum length
			index = i;			// index of queue with maximum length
		}
		if( index==_rrCounter ) {
			queueIndex = index;
		}
	}

	_rrCounter--;

	if( queueIndex!=-1)
		return queueIndex;

	//cout << "queueIndex: " << queueIndex << endl;

#else

	// remember which priority queue was chosen last ->RR manner (closer to literature, kurose09)
	if (_rrCounter < 0)	// reset
		_rrCounter = _nofCoS-1;

	if( _wfq_counter[_rrCounter] == _wfq_weight[_rrCounter] ) {	// reset queues counter
		_wfq_counter[_rrCounter] = 0;
	}
	if( _wfq_counter[_rrCounter] < _wfq_weight[_rrCounter] ) {
		if (getQueue(_rrCounter)->length() > 0) { // try up to wfq_weight[i] times
			queueIndex = _rrCounter;
			_rrCounter = queueIndex;
			_wfq_counter[_rrCounter]++;
		} else {
			queueIndex = -1;
			_wfq_counter[_rrCounter]++;
			_rrCounter--;
		}
		//wfq_counter[_rrCounter]++;
		if( queueIndex!=-1)
			return queueIndex;
	}
	//cout << "wfq chosen: " << queueIndex << " wfq_counter[q] " << wfq_counter[queueIndex] << endl;
	//cout << "wfq chosen: " << queueIndex << " counter7 " << _counter7 << endl;
#endif
	return queueIndex;
} // WeightedFairQueuingRR()

int Scheduler::WeightedFairQueuingHP() {
	int queueIndex = -1;

#if 1
	// leads to similar results as implementation below
	int weights[_nofCoS];

	// initialize weights array
	for( int i=_nofCoS-1; i>=0; i-- ) {
		weights[i] = 0;
	}
	double usedUp = 100.0;	// remember how much of 100% available bandwidth is used up

	for( int i=_nofCoS-1; i>=0; i-- ) {
		if( getQueue(i)->length()>0 ) {
			// distribute weights equally among non-empty priority queues (100%)	-> bad results
			//weights[i] = ceil(100.0/double(nonEmptyQueues));

			// calculate actual weights used based on non-empty priority queues and maximum share of bandwidth
			weights[i] = ceil( ((double(_wfq_weight[i]))/100.0)*usedUp );	// distribute weights according to max allowed percentage of bandwidth
			//cout << i << ": confWeight " << _wfq_weight[i] << ", calcWeight " << weights[i] << " usedUp: " << usedUp << endl;
			usedUp -= weights[i];
		}
	}

	// find index of maximum weight in weights array
	queueIndex = findMaxInArray(weights, _nofCoS);
	//cout << "queueIndex: " << queueIndex << endl;

#else
	// don't remember the last queue chosen since queues states may have changed,
	// start again from highest priority (similar to Priority scheduling)
	for(int i=_nofCoS-1; i>-1; i-- ) {
		if( _wfq_counter[i] == _wfq_weight[i] ) {
			_wfq_counter[i] = 0;
		}
		if( _wfq_counter[i] < _wfq_weight[i] ) {
			if (getQueue(i)->length() > 0) { // try up to wfq_weight[i] times
				queueIndex = i;
				_wfq_counter[i]++;
			} else {
				queueIndex = -1;
				_wfq_counter[i]++;
			}

			if( queueIndex!=-1)
				break;
		}
	}
#endif
	//cout << "wfq chosen: " << queueIndex << " wfq_counter[q] " << wfq_counter[queueIndex] << endl;
	//cout << "wfq chosen: " << queueIndex << " counter7 " << _counter7 << endl;
	return queueIndex;
} // WeightedFairQueuingHP()

int Scheduler::FairQueueSizebasedWeighting() {

	// consider only queues with contents
	// assign weights according to priority and fullness
	// maybe similar to DRR, credits according to bandwidth
	// watch queue capacity, when getting too full assign higher weight
	int queueIndex =-1;
#if 0
	// try to re-implement in a simpler way
#else
	map<int, int>::iterator mit;
	_mapQSizes.clear();
	_highestIndex.clear();
	// sort queues in ascending order according to their length
	// shortest queue will be in the beginning, longest queue will be in the end

	for (int i = (_nofCoS-1); i > -1; i--) {
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
#endif
	return queueIndex;
} // FairQueueSizebasedWeighting()

int Scheduler::FirstComeFirstServed() {
	int queueIndex=-1;
	map< double, int >::iterator sit;
	string qname;
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
	int queueIndex = 0;
	int maxi = 0;

	// find queue with maximum length
	for(int i = _nofCoS-2; i >= 0; i--) {
		if( maxi < queuelengths[i] ) {
			maxi = queuelengths[i];	// maximum length
			queueIndex = i;			// index of queue with maximum length
		}
	}
	return queueIndex;
} // findMaxQLengthIndex()

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

	_weight[priority] = priority_weight * (mit->second);

	// adjust to bandwidth of 1 Gbps
	_queue_credit[priority] = (1000 * _weight[priority]);	// re-assign queue credit (is this a good idea to keep the old values? in the next cycle queue lengths will have changed)
	//cout << " queue_credit["<<priority<<"] " << queue_credit[priority] << " weight["<<priority<<"] " << weight[priority] << " credit_cnt " << credit_counter[priority] << endl;

	if (_credit_counter[priority] == 0)
		_credit_counter[priority] = _queue_credit[priority];
	int packet_size = getQueue(priority)->front()->getByteLength();
	_credit_counter[priority] -= packet_size + _ifgBytes;

	// select queue according to available queue credit
	if ((packet_size + _ifg) <= _credit_counter[priority]) {
		queueIndex = priority;
		_credit_counter[priority] -= packet_size + _ifgBytes;
	} else {
		// give more credit
		_credit_counter[priority] += _queue_credit[priority];
		queueIndex = -1;
	}
	return queueIndex;
} // determineQIndex()

int Scheduler::findMaxInArray( int array[], int arraysize ) {
	int index = 0;
	int maxi = 0;

	// find queue with maximum length
	for(int i = 0; i < arraysize; i++) {
		if( maxi < array[i] ) {
			maxi = array[i];	// maximum length
			index = i;			// index of queue with maximum length
		}
	}
	return index;
} // findMaxInArray()

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

