//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


namespace qlib;

cplusplus {{
#include "QueueingDefs.h"
}}

//
// Packet definition for queueing framework
//
packet WRPacket
{
    @customize(true);
    int priority;                // queueing priority
    simtime_t totalQueueingTime; // total time spent standing in queues
    simtime_t totalServiceTime;  // total time spent in servers
    simtime_t totalDelayTime;    // total time spent in delay modules
    int queueCount;              // the number of queue modules visited by the packet
    int delayCount;              // the number of delay modules visited by the packet
    int generation;              // how many times the original ancestor was copied
    int operationCounter;			// counts the operations on a packet during scheduling
}


