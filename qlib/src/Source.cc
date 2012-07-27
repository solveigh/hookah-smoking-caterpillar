//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Source.h"
#include "Packet_m.h"

namespace qlib {

void SourceBase::initialize() {
	createdSignal = registerSignal("created");
	jobCounter = 0;
	WATCH(jobCounter);
}

WRPacket *SourceBase::createJob() {
	// TODO check me - not used!?
	char buf[80];
	sprintf(buf, "%.60s-%d", "p", ++packetCounter);
	WRPacket *packet = new WRPacket(buf);
	int prio = Useful::getInstance()->generateRandomPriority();
	packet->setPriority(prio); //par("packetPriority"));
	return packet;
}

void SourceBase::finish() {
	// remove all indisposed messages in the end!
	this->setPerformFinalGC(true);
	emit(createdSignal, jobCounter);
}

//----

Define_Module(Source);

void Source::initialize() {
	SourceBase::initialize();
	startTime = par("startTime");
	numPackets = par("numPackets");

	_nofCoS = par ("nofCoS");
	cout << "Source nofCoS " << _nofCoS << endl;

	_interArrivalTime = par("interArrivalTime");
	_ifg = par("ifg");
	
	// schedule the first message timer for start time
	scheduleAt(startTime, new cMessage("newJobTimer"));

	startSendingPacket = new cMessage("startSendingPacket");

	_inputDataFile = par("inputDataFile").stringValue();
	_data = Useful::getInstance()->readDataList(_inputDataFile);
	//Useful::getInstance()->appendToFile("out.txt", _inputDataFile.c_str());
	//Useful::getInstance()->appendToFile("source_out.txt", int(_data.size()));
	while (_data.size() < numPackets) {
		std::vector<PacketDescription> v = Useful::getInstance()->readDataList(
				_inputDataFile);
		_data.insert(_data.end(), v.begin(), v.end());
	}
	//Useful::getInstance()->appendToFile("source_out.txt", int(_data.size()));

	//for( int i=0; i<_data.size(); i++ )
	//std::cout << "i: " << i << " " << _data.at(i).getPriority() << " " << _data.at(i).getSize() << std::endl;

	WATCH(numCreated);
	numCreated = 0;

	for( int i=0; i<_nofCoS; i++ ) {
		_sent.push_back(0);
	}

	outQueue = new cQueue();
	cout << " Start: " << simTime() << endl;

} // initialize()

void Source::handleMessage(cMessage *msg) {
	ASSERT(msg->isSelfMessage());

    if( (numCreated < numPackets) ) { //&& (stopTime < 0 || stopTime > simTime()) ) {

        // reschedule the timer for the next message
		// consider interArrivalTime and ifg
		simtime_t nextPacket = _interArrivalTime + _ifg;
        scheduleAt(simTime() + nextPacket, msg);

        if( outQueue->length()>0 ) {
        	WRPacket *packet = check_and_cast<WRPacket*>(outQueue->pop());
        	sendPacket(packet);
        }
        else if (_data.size() > _nofCoS) {
        	vector<PacketDescription>::iterator it;
			it = _data.begin();
			if (it != _data.end()) {
				int prio = (*it).getPriority();
				WRPacket *p = generatePacket(prio, (*it).getSize());
				numCreated++;
				sendPacket(p);
				_data.erase(it);
			}
        }
    } else {
        // finished
        delete msg;
    }
} // handleMessage()

int Source::sendPacket(WRPacket* packet) {
	int prio = packet->getPriority();
	cGate* outputgate = getGate(prio);
	cDatarateChannel* channel = check_and_cast<cDatarateChannel *> (outputgate->getTransmissionChannel());
	simtime_t t = channel->calculateDuration(packet);
	simtime_t txFinishTime = channel->getTransmissionFinishTime();
	//cout << "duration " << packet->getBitLength() << "/" << channel->getDatarate() << " = " << packet->getBitLength()/channel->getDatarate()  << " " << t << endl;
	//cout << "finish: " << txFinishTime << " sim: " << simTime() << " packet: " << packet->getName() << " size: " << packet->getBitLength() << " bit, " << packet->getByteLength() << " byte."<< endl;

	if( txFinishTime <= simTime() || !channel->isBusy() ) {
		// channel free; send out packet immediately
		send(packet, outputgate);
		_sent.at(prio) = _sent.at(prio)+1;
	} else {
		// store packet and schedule timer; when the timer expires,
		// the packet should be removed from the queue and sent out
		outQueue->insert(packet);
		//cout << "outQueue: " << outQueue->length() << endl;
	}

	return _sent.at(prio);
} // sendPacket()

WRPacket * Source::generatePacket(int priority, int size) {
	//log("test");
	char buf[10];
	sprintf(buf, "%.60s-%d", "p", ++packetCounter);
	buf[9] = '\0';
	WRPacket *packet = new WRPacket(buf);
	packet->setPriority(priority);
	packet->setByteLength(size);

	simtime_t creationTime = simTime();
	char name[80];
	//sprintf(name, "id: %ld, priority: %d; %f", packet->getId(), random, triggerTime);
	sprintf(name, "id: %ld, priority: %d; > %lf", packet->getId(), priority,
			creationTime.dbl());
	name[79] = '\0';
	packet->setName(name);	// set proper name
	//std::cout << "packet (id: " << packet->getId() << ") priority set to: " << priority << " size " << size << std::endl;

	packet->setTimestamp(creationTime);

	return packet;
} // generatePacket()


cGate * Source::getGate(int index) {
	cGate *outputgate = gate("pppg", index);
	return outputgate;
} // getGate()

//----

Define_Module(SourceOnce);

void SourceOnce::initialize() {
	SourceBase::initialize();
	simtime_t time = par("time");
	scheduleAt(time, new cMessage("newJobTimer"));
}

void SourceOnce::handleMessage(cMessage *msg) {
	ASSERT(msg->isSelfMessage());
	delete msg;

	int n = par("numJobs");
	for (int i = 0; i < n; i++) {
		WRPacket *job = createJob();
		send(job, "out");
	}
}

}
;
//namespace

