
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
#include <algorithm>

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
	    string schedulingAlgorithm;
        int routingAlgorithm;  // the algorithm we are using for scheduling

        cDatarateChannel *channel;	// reference to the output channel
        IPassiveQueue *getQueue(int index);

        simtime_t startTime;	// start-time of the simulation
        simtime_t serviceTime;	// emulate internal clock cycle of WRS

        cMessage *triggerServiceMsg;	// receive trigger messages

        int _nofCoS;	// default: 7..0
        IPassiveQueue *_q7;	// reference to priority queue 7
        IPassiveQueue *_q2;	// reference to priority queue 2

        // pointer to other queues
        std::vector<IPassiveQueue*> _qs;

        void determineQueueSizes();	// print queue's capacities

        map<int, int> _mapQSizes;	// length, queue index

		simtime_t _ifg;	// Time for the Interframe Gap

		// RR / WRR
		int _rrCounter;         // counter for round robin scheduling

        // WFQ
		int wfq_weight[8];	// only 0..3 are used for _nofCoS=3
		int wfq_counter[8];	// only 0..3 are used for _nofCoS=3

        int _weight7;
        int _weight6;
        int _weight5;
        int _weight4;
        int _weight3;
        int _weight2;
        int _weight1;
        int _weight0;

        int _counter7;


        int _counter6;	// TODO still in use?
        int _counter5;
        int _counter4;

        int _counter2;
        int _counter1;
        int _counter0;


        // FCFS
        string getQueueName(int index);
        map<double,int> _mapPacketAges;

		bool file_delete(std::string filename) {
			if( unlink( filename.c_str() ) != 0)
				return false;
			return true;
		}

        // WRR
        int sumWeights(int weight[], int size);
        int calculateMaxWeight(int weight[], int asize);
        int calculateHighestCommonDivisor(int weight[], int asize);
        int queue_credit[8];	// only 0..3 are used for _nofCoS=3
    	int credit_counter[8];	// only 0..3 are used for _nofCoS=3
    	int weight[8];	// only 0..3 are used for _nofCoS=3
    	int _ifgBytes;	// Interframe Gap

        // FQSW
    	// calculate queue credits dynamically, return queue index if queue has enough credit
        int determineQIndex(map<int, int>::iterator mit, int priority);
        set<int> _highestIndex;

        // LQF+
        int findMaxQLengthIndex( int queuelengths[] );

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

    public:
        string getSchedulingAlgorithm() {
        	return schedulingAlgorithm;
        }
};

}; //namespace

#endif
