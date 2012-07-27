//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_PACKETLIST_H
#define __QUEUEING_PACKETLIST_H

#include "QueueingDefs.h"
#include <set>

namespace qlib {

/**
 * Makes it possible to iterate over all Packet messages in the system.
 */
class QUEUEING_API PacketList : public cSimpleModule
{
    friend class WRPacket;
    protected:
        std::set<WRPacket*> packets;
        static PacketList *defaultInstance;
    public:
        PacketList();
        ~PacketList();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        void registerPacket(WRPacket *packet);
        void deregisterPacket(WRPacket *packet);
    public:
        /**
         * Returns pointer to the default instance of PacketList, which is
         * the first PacketList module in the network. Returns NULL if
         * there's no PacketList module.
         */
        static PacketList *getDefaultInstance();

        /**
         * Returns the pointers of the packets currently existing in the model.
         */
        const std::set<WRPacket*> getPackets();
};

}; // namespace

#endif


