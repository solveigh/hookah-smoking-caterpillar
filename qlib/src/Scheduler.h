
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
     ALG_WFQ_RR,
     ALG_WFQ_HP,
     ALG_WRR,
     ALG_FQSW
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

        int _nofCoS;	// K=8 / K=3

        // PRIO
        int Priority();

		// RR / WRR
        int RoundRobin();
		int _rrCounter;         // counter for round robin scheduling

        // WFQ
		int WeightedFairQueuingRR();
		int WeightedFairQueuingHP();
		int _wfq_weight[8];	// only 0..3 are used for _nofCoS=3
		int _wfq_counter[8];	// only 0..3 are used for _nofCoS=3

        // WRR
        int WeightedRoundRobin();
        int sumWeights(int weight[], int size);
        int calculateMaxWeight(int weight[], int asize);
        int calculateHighestCommonDivisor(int weight[], int asize);
        int _queue_credit[8];	// only 0..3 are used for _nofCoS=3
    	int _credit_counter[8];	// only 0..3 are used for _nofCoS=3
    	int _weight[8];	// only 0..3 are used for _nofCoS=3

    	int _ifgBytes;	// Interframe Gap
		simtime_t _ifg;	// Time for the Interframe Gap

        // FQSW
		int FairQueueSizebasedWeighting();
    	// calculate queue credits dynamically, return queue index if queue has enough credit
        int determineQIndex(map<int, int>::iterator mit, int priority);
        set<int> _highestIndex;
        void determineQueueSizes();	// print queue's capacities
        map<int, int> _mapQSizes;	// length, queue index

        // LQF+
        int LongestQueueFirstPlus();
        int findMaxQLengthIndex( int queuelengths[] );

        // FCFS
        int FirstComeFirstServed();
        string getQueueName(int index);
        map<double,int> _mapPacketAges;

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
