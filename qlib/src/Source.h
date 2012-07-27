//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_SOURCE_H
#define __QUEUEING_SOURCE_H

#include "QueueingDefs.h"
#include "Sink.h"
#include "Packet_m.h"
#include "Useful.h"
#include "WRPacket.h"

namespace qlib {

//class Sink;
class WRPacket;

/**
 * Abstract base class for job generator modules
 */
class QUEUEING_API SourceBase : public cSimpleModule
{
    protected:
        int jobCounter;
        simsignal_t createdSignal;
        int packetCounter;

    protected:
        virtual void initialize();
        virtual WRPacket *createJob();
        virtual void finish();
};


/**
 * Generates jobs; see NED file for more info.
 */
class QUEUEING_API Source : public SourceBase
{
    private:
        simtime_t startTime;
        simtime_t stopTime;
        int numPackets;
        int numCreated;
        std::vector<PacketDescription> _data;

        std::vector<int> _sent;
        string _inputDataFile;

        int _nofCoS;

        cQueue *outQueue;

        cGate * getGate(int index);

        cMessage *receivePacket;
        cMessage *startSendingPacket;
		
		// Time between packets
		simtime_t _interArrivalTime;
		
		// Time for the Interframe Gap
		simtime_t _ifg;

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

        WRPacket * generatePacket( int priority, int size );
        int sendPacket(WRPacket* packet);

    public:
    	int getCreated() { return numCreated; };
    	int getNumPackets() { return numPackets; };
    	std::vector<int> getSent() { return _sent; };
    	string getInputDataFileName() { return _inputDataFile; };
};


/**
 * Generates jobs; see NED file for more info.
 */
class QUEUEING_API SourceOnce : public SourceBase
{
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

}; //namespace

#endif


