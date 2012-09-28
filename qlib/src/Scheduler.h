
#ifndef __QUEUEING_SCHEDULER_H
#define __QUEUEING_SCHEDULER_H

#include "QueueingDefs.h"
#include "WRPacket.h"
#include "IPassiveQueue.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <set>

using std::cout;
using std::endl;
using std::map;
using std::set;
using std::vector;
using std::string;
using std::pair;
using std::ifstream;
using std::ofstream;
using std::ios;

namespace qlib {

// routing algorithms
enum alg {
	 ALG_PRIO,
	 ALG_RR,
     ALG_LQFP,
     ALG_DRRP,
     ALG_WRQ,
     ALG_WFQP
};

/**
 * Sends the messages to different outputs depending on a set algorithm.
 */
class QUEUEING_API Scheduler : public cSimpleModule
{
    private:
	    string _schedulingAlgorithm;
        int _routingAlgorithm;  // the algorithm that is used for scheduling

        cDatarateChannel *_channel;	// reference to the output channel
        IPassiveQueue *getQueue(int index);

        simtime_t _startTime;	// start-time of the simulation
        simtime_t _serviceTime;	// emulate internal clock cycle of WRS

        cMessage *_triggerServiceMsg;	// receive trigger messages

        int _nofPriorityClasses;	// K=8 / K=3

        // PRIO
        int Priority();

		// RR / DRR
        int RoundRobin();
		int _rrIndex;         // counter for round robin scheduling

        // WRQ
		int WeightedRandomQueuing();
		int _wrq_weight[8];	// only 0..2 are used for N=3

		// WFQ
		int WeightedFairQueuingPlus();
		int _wfq_weight[8];	// only 0..2 are used for N=3
		int _wfq_counter[8]; // only 0..2 are used for N=3

        // DRRP
        int DeficitRoundRobinPlus();
        int _drr_weight[8];	// only 0..2 are used for N=3
        int _drr_counter[8];	// only 0..2 are used for N=3
    	int _bandwidth;	// available bandwidth of the network

    	int _ifgBytes;	// Interframe Gap
		simtime_t _ifg;	// Time for the Interframe Gap

        // LQF+
        int LongestQueueFirstPlus();

		bool file_delete(std::string filename) {
			if( unlink( filename.c_str() ) != 0)
				return false;
			return true;
		}

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

    public:
        string getSchedulingAlgorithm() {
        	return _schedulingAlgorithm;
        }
};

}; //namespace

#endif
