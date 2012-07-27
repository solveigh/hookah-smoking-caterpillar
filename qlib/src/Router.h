//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_ROUTER_H
#define __QUEUEING_ROUTER_H

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
	 ALG_RR,
     ALG_SQF,
     ALG_LQF,
     ALG_WFQ,
     ALG_FCFS,
     ALG_WRR,
     ALG_FQSW
};

/**
 * Sends the messages to different outputs depending on a set algorithm.
 */
class QUEUEING_API Router : public cSimpleModule
{
    private:
	    string schedulingAlgorithm;
        int routingAlgorithm;  // the algorithm we are using for routing
        int rrCounter;         // msgCounter for round robin routing

        cDatarateChannel *channel;
        IPassiveQueue *getQueue(int index);
        simtime_t serviceTime;
        // receive trigger messages
        cMessage *triggerServiceMsg;

        int _nofCoS;	// default: 7..0
        IPassiveQueue *_q7;

        IPassiveQueue *_q2;

        // pointer to other queues
        std::vector<IPassiveQueue*> _qs;

        void determineQueueSizes();

        map<int, int> _mapQSizes;	// length, queue index

        // wfq
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

        // priority
        int _priorityCounter;

        // fcfs
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
    	int _packet_size;	// size of the payload of a packet
    	int _ifg;	// Interframe Gap

        int determinOperationCount(int routingAlgorithm);

        // my
        int determineQIndex(map<int, int>::iterator mit, int priority);
        set<int> _highestIndex;

        // fcfs
        string getQueueName(int index);

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
