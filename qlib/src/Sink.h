//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_SINK_H
#define __QUEUEING_SINK_H

#include "QueueingDefs.h"
#include "IPassiveQueue.h"
#include "PassiveQueue.h"
#include "Packet_m.h"
#include "Router.h"
#include "Source.h"

#include "Useful.h"
#include <vector>
#include <stdio.h>
#include <string>

using namespace std;

namespace qlib {

class WRPacket;
class Source;

/**
 * Consumes jobs; see NED file for more info.
 */
class QUEUEING_API Sink : public cSimpleModule
{
  private:
	simsignal_t lifeTimeSignal;
	simsignal_t totalQueueingTimeSignal;
	simsignal_t queuesVisitedSignal;
	simsignal_t totalServiceTimeSignal;
	simsignal_t totalDelayTimeSignal;
	simsignal_t delaysVisitedSignal;
	simsignal_t generationSignal;
    bool keepJobs;

    int _nofCoS;

    int numReceived;

    void determineQueueSizes();

    // group for transfer times
	vector<simtime_t> v0;
	vector<simtime_t> v1;
	vector<simtime_t> v2;
	vector<simtime_t> v3;
	vector<simtime_t> v4;
	vector<simtime_t> v5;
	vector<simtime_t> v6;
	vector<simtime_t> v7;
	double avg_lifetime(vector<simtime_t> v);	// return time in us

	// Total queuing times
	vector<simtime_t> vq0;
	vector<simtime_t> vq1;
	vector<simtime_t> vq2;
	vector<simtime_t> vq3;
	vector<simtime_t> vq4;
	vector<simtime_t> vq5;
	vector<simtime_t> vq6;
	vector<simtime_t> vq7;

	double avg_lifetime(vector<double> v);	// return time in us

	// Total service times
	vector<simtime_t> vs0;
	vector<simtime_t> vs1;
	vector<simtime_t> vs2;
	vector<simtime_t> vs3;
	vector<simtime_t> vs4;
	vector<simtime_t> vs5;
	vector<simtime_t> vs6;
	vector<simtime_t> vs7;

	// scheduling times
	vector<simtime_t> vsch0;
	vector<simtime_t> vsch1;
	vector<simtime_t> vsch2;
	vector<simtime_t> vsch3;
	vector<simtime_t> vsch4;
	vector<simtime_t> vsch5;
	vector<simtime_t> vsch6;
	vector<simtime_t> vsch7;

	// pointer to queues
	std::vector<PassiveQueue*> _qs;
	PassiveQueue *getQueue(int index);

	Source *_source;
	Router *_router;

	void write2File(string filename);
	void writeDropped2File(string filename);
	void writeDropped2FilePercentage(string filename);
	void writeTimes2File(string filename);
	void writeQueuingSchedulingTimes2File(string filename);

	void writeTimes2File4Plot(string filename);
	void writeQueuingTimes2File4Plot(string filename);
	void writeDropped2File4Plot(string filename);
	void writeDropped2File4PlotPercentage(string filename);

	bool file_delete(std::string filename) {
		if( unlink( filename.c_str() ) != 0)
			return false;
		return true;
	}

	simtime_t calculateSchedulingTime(simtime_t source, simtime_t sink, simtime_t queue);

	void canFinish();

	void writeNumbers2TexFile(string filename);

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

  public:
    int getNumReceived() { return numReceived; };

};

}; //namespace

#endif

