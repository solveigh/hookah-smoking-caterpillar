//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <algorithm>
#include "WRPacket.h"
#include "PacketList.h"

namespace qlib {

WRPacket::WRPacket(const char *name, int kind, PacketList *packetList) : WRPacket_Base(name, kind)
{
    parent = NULL;
    if (packetList==NULL && PacketList::getDefaultInstance()!=NULL)
        packetList = PacketList::getDefaultInstance();
    this->packetList = packetList;
    if (packetList!=NULL)
        packetList->registerPacket(this);
}

WRPacket::WRPacket(const WRPacket& packet)
{
    setName(packet.getName());
    operator=(packet);
    parent = NULL;
    packetList = packet.packetList;
    if (packetList!=NULL)
        packetList->registerPacket(this);
}

WRPacket::~WRPacket()
{
    if (parent)
        parent->childDeleted(this);
    for (int i=0; i<(int)children.size(); i++)
        children[i]->parentDeleted();
    if (packetList!=NULL)
        packetList->deregisterPacket(this);
}

WRPacket& WRPacket::operator=(const WRPacket& packet)
{
    if (this==&packet) return *this;
    WRPacket_Base::operator=(packet);
    // leave parent and packetList untouched
    return *this;
}

WRPacket *WRPacket::getParent()
{
    return parent;
}

void WRPacket::setParent(WRPacket *parent)
{
    this->parent = parent;
}

int WRPacket::getNumChildren() const
{
    return children.size();
}

WRPacket *WRPacket::getChild(int k)
{
    if (k<0 || k>=(int)children.size())
        throw cRuntimeError(this, "child index %d out of bounds", k);
    return children[k];
}

void WRPacket::makeChildOf(WRPacket *parent)
{
    parent->addChild(this);
}

void WRPacket::addChild(WRPacket *child)
{
    child->setParent(this);
    ASSERT(std::find(children.begin(), children.end(), child)==children.end());
    children.push_back(child);
}

void WRPacket::parentDeleted()
{
    parent = NULL;
}

void WRPacket::childDeleted(WRPacket *child)
{
    std::vector<WRPacket*>::iterator it = std::find(children.begin(), children.end(), child);
    ASSERT(it!=children.end());
    children.erase(it);
}

}; // namespace
