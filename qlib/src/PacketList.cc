//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "WRPacket.h"
#include "PacketList.h"

namespace qlib {

Define_Module(PacketList);

PacketList *PacketList::defaultInstance = NULL;


PacketList::PacketList()
{
    if (defaultInstance == NULL)
        defaultInstance = this;
}

PacketList::~PacketList()
{
    if (defaultInstance == this)
        defaultInstance = NULL;
    for (std::set<WRPacket*>::iterator it=packets.begin(); it!=packets.end(); ++it)
        (*it)->packetList = NULL;
}

void PacketList::initialize()
{
    WATCH_PTRSET(packets);
}

void PacketList::handleMessage(cMessage *msg)
{
    throw cRuntimeError("this module does not process messages");
}

void PacketList::registerPacket(WRPacket *packet)
{
    packets.insert(packet);
}

void PacketList::deregisterPacket(WRPacket *packet)
{
    std::set<WRPacket*>::iterator it = packets.find(packet);
    ASSERT(it != packets.end());
    packets.erase(it);
}

PacketList *PacketList::getDefaultInstance()
{
    return defaultInstance;
}

const std::set<WRPacket*> PacketList::getPackets()
{
    return packets;
}

}; // namespace
