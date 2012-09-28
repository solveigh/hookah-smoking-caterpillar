
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
		_rrIndex = 0;
	} else if (strcmp(algName, "LQF+") == 0) {
		_routingAlgorithm = ALG_LQFP;
		_schedulingAlgorithm = "LQFP";
	} else if (strcmp(algName, "WRQ") == 0) {
		_routingAlgorithm = ALG_WRQ;
		_schedulingAlgorithm = "WRQ";
	} else if (strcmp(algName, "DRR+") == 0) {
		_routingAlgorithm = ALG_DRRP;
		_schedulingAlgorithm = "DRRP";
		_rrIndex = 0;
	} else if (strcmp(algName, "WFQ+") == 0) {
		_routingAlgorithm = ALG_WFQP;
		_schedulingAlgorithm = "WFQP";
		_rrIndex = 0;
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
	for (int i = 0; i < _nofPriorityClasses; i++)
		_drr_counter[i] = 0;

	// WRQ
	// rand() returns always the same sequence of random numbers
	srand(0);	// seed rand() with the same number to be able to compare the same algorithm in

	// WFQ+
	for (int i = 0; i < _nofPriorityClasses; i++)
		_wfq_counter[i] = 0;

	// Interframe Gap
	_ifg = par("ifg");
	_ifgBytes = 12; //_ifg * 1250*10^9; 1250000000000
} // initialize()

void Scheduler::handleMessage(cMessage *msg) {
	int queueIndex = -1; // by default we drop the packet

	if (msg == _triggerServiceMsg) {
		// a trigger event arrived (either packet transmission finished or simulated cycle event)
		switch (_routingAlgorithm) {

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

int Scheduler::Priority() {
	int queueIndex=-1;
	// go through all priority queues until you find a packet,
	// start with highest priority
	for( int index = _nofPriorityClasses-1; index>=0; index-- ) {
		if( getQueue(index)->length()>0 ) {
			queueIndex = index;
			break;	// leave for-loop
		}
	}

	return queueIndex;
} // Priority()

int Scheduler::RoundRobin() {

	// work-conserving Round-Robin
	int queueIndex = -1;
	int index=0;
	for( int i=_nofPriorityClasses-1; i>=0; i-- ) {
		index = (i+_rrIndex) % _nofPriorityClasses;	// calculate index to start from
		if( getQueue(index)->length()>0 ) {
			queueIndex = index;
			_rrIndex = index;	// remember as start-point for next cycle
			break;
		}
	}
	return queueIndex;
} // RoundRobin()

int Scheduler::LongestQueueFirstPlus() {
	int queueIndex = -1;

	if( getQueue(_nofPriorityClasses-1)->length()>0 ) {		// highest priority queue
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

int Scheduler::DeficitRoundRobinPlus() {
	int queueIndex = -1;

   // work-conserving DRR+
	if( getQueue(_nofPriorityClasses-1)->length() > 0 ) {    // treat highest priority class separately
		queueIndex = _nofPriorityClasses-1;
	} else {    // treat standard priority queues fairly in round-robin manner
		int index=0;
		for( int i=_nofPriorityClasses-2; i>=0; i-- ) {
			index = (i+_rrIndex) % (_nofPriorityClasses-1);	// calculate index to start from

	        if( getQueue(index)->length() > 0 ) {    // consider only non-empty queues
	            if( _drr_counter[index]==0 )
	            	_drr_counter[index] = _drr_weight[index];    // restore credit counter to allowed quantum of bandwidth

	        	int packet_size = getQueue(index)->front()->getByteLength(); // packet size in bytes of oldest packet in queue

	            if( packet_size <= _drr_counter[index] ) {  // if queue's credit is available
	                queueIndex = index;  // select this queue for next transmission
	                _drr_counter[index] -= packet_size;   // decrement credit counter of this queue by the next removed packet's size
	                break; // leave for-loop
	            } else {
	                // give more credit to a queue if necessary
	                _drr_counter[index] += _drr_weight[index];
	                _rrIndex = index;
	            }
	        } else {
	        	_drr_counter[index] = 0;	// reset queue credit to 0
	            _rrIndex = index;
	        }
		}
	}
	return queueIndex;
} // DeficitRoundRobinPlus()

int Scheduler::WeightedFairQueuingPlus() {
	int queueIndex = -1;

    // work-conserving WFQ+
    if( getQueue(_nofPriorityClasses-1)->length() > 0 ) {    // treat highest priority class separately
        queueIndex = _nofPriorityClasses-1;
    } else {    // treat standard priority queues fairly
    	int index=0;
		for( int i=_nofPriorityClasses-2; i>=0; i-- ) {
			index = (i+_rrIndex) % (_nofPriorityClasses-1);	// calculate index to start from

			if( getQueue(index)->length() > 0 ) {    // consider only non-empty queues
				if (_wfq_counter[index] == 0)
					_wfq_counter[index] = _wfq_weight[index];    // restore credit counter to number of packets

				if( _wfq_counter[index]>0 ) {  // if queue's credit is available and queue is not empty
					queueIndex = index;  // select this queue for next transmission
					_wfq_counter[index] -= 1;   // decrement credit counter of this queue by the next removed packet
					break; // leave for-loop
				} else {
					// give more credit to a queue if necessary
					_wfq_counter[index] += _wfq_weight[index];
					_rrIndex = index; // store index for next round
				}
			} else {
				_wfq_counter[index] = 0;  // reset credit counter
				_rrIndex = index; // store index for next round
			}
		}
    }

	return queueIndex;
}	// WeightedFairQueuingPlus()

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

}
;
//namespace

