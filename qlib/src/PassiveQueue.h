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
#include "Router.h"
#include "WRPacket.h"

#include <vector>

using std::vector;

namespace qlib {

class WRPacket;

/**
 * A passive queue, designed to co-operate with IServer using method calls.
 */
class QUEUEING_API PassiveQueue : public cSimpleModule, public IPassiveQueue
{
    private:
		simsignal_t droppedSignal;
		simsignal_t queueLengthSignal;
		simsignal_t queueingTimeSignal;

        bool fifo;
        int _capacity;
        int _size;
        cQueue queue;

        void queueLengthChanged();

        void enqueue(cPacket* msg);

        vector<WRPacket* > _dropped;

        cModule * router;

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

    public:
        PassiveQueue();
        virtual ~PassiveQueue();
        // The following methods are called from IServer:
        virtual int length();
        virtual int size();
        virtual void request(int gateIndex);

        WRPacket * front();

        cQueue getQueue() { return queue; };

        vector<WRPacket* > getDropped() {return _dropped;};
};

}; //namespace

#endif

