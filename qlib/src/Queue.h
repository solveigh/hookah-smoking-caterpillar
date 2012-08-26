//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_PASSIVE_QUEUE_H
#define __QUEUEING_PASSIVE_QUEUE_H

#include "QueueingDefs.h"
#include "IPassiveQueue.h"
#include "Packet_m.h"
#include "Scheduler.h"
#include "WRPacket.h"

#include <vector>

using std::vector;

namespace qlib {

class WRPacket;

/**
 * A passive queue, designed to co-operate with IServer using method calls.
 */
class QUEUEING_API Queue : public cSimpleModule, public IPassiveQueue
{
    private:
		simsignal_t droppedSignal;
		simsignal_t queueLengthSignal;
		simsignal_t queueingTimeSignal;

        bool fifo;						// true: queue acts in FCFS mode
        int _capacity;					// queue capacity (number of positions for memory addresses)
        int _size;
        cQueue queue;					// the queue itself

        void queueLengthChanged();

        void enqueue(cPacket* msg);		// store packets in queue as long as capacities last

        vector<WRPacket* > _dropped;	// keep track of dropped packets

        cModule * scheduler;			// access to scheduler module

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

    public:
        Queue();
        virtual ~Queue();

        // The following three methods are called from IServer:
        virtual int length();					// queue length (number of packets)
        virtual int size();						// queue length (amount of packet sizes in bytes)
        virtual void request(int gateIndex);	// request a packet from the queue


        WRPacket * front();						// pointer to oldest packet in queue

        cQueue getQueue() { return queue; };

        vector<WRPacket* > getDropped() {return _dropped;};

        int getCapacity () { return _capacity; };
};

}; //namespace

#endif

