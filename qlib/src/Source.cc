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
	
	_serviceTime = par("serviceTime");

	_burstTest = par("burstTest");
	if( _burstTest ) {
		_burstCounter=0;
		_nofBurstCounter=0;
		Useful::getInstance()->setBurstIntervals();
		_saveBurstData=false;
		saveBurstDataMsg = new cMessage("saveBurstData");
	} else {
		_burstCounter=-1;
		_nofBurstCounter=-1;
		_saveBurstData=false;
	}

	// schedule the first message timer for start time
	startSendingPacket = new cMessage("startSendingPacket");
	scheduleAt(startTime, startSendingPacket);

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

	// synchronous sending of packets
    if( (numCreated < numPackets) ) {

    	if( _burstTest == false ) {
    		// generate next packet when transmission is finished
    		if( msg == startSendingPacket ) {
				if (_data.size() > 0) {
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
    		}
    	} else {
    		// investigate satiation behavior
    		// TODO sent 10,20,40,80,160,320,640,... packets, after each interval collect data
    		// generate next packet when transmission is finished
    		if( msg == startSendingPacket ) {
				if (_data.size() > 0) {
					vector<PacketDescription>::iterator it;
					it = _data.begin();

					int prio = (*it).getPriority();	// always remember the last
					WRPacket *p = generatePacket(prio, (*it).getSize());

					if (it != _data.end() && _nofBurstCounter!=Useful::getInstance()->getBurstIntervals().at(_burstCounter) ) {
						//cout << "simTime() " << simTime() << endl;
						sendPacket(p);
						_data.erase(it);
						numCreated++;
						_nofBurstCounter++;
						if(_saveBurstData==true) _saveBurstData=false;
					}
					if( _nofBurstCounter==Useful::getInstance()->getBurstIntervals().at(_burstCounter) ){
						// wie sicher stellen dass source und sink zum selben event dran kommen???
						//cout << "lala " << endl;
						// schedule next burst interval
						/*cancelEvent(startSendingPacket);

						cGate* outputgate = getGate(prio);
						cDatarateChannel* channel = check_and_cast<cDatarateChannel *> (outputgate->getTransmissionChannel());
						simtime_t t = channel->calculateDuration(p);

						scheduleAt((simTime()+t), startSendingPacket);	// TODO waiting time? // This affects the results!!!
						*/
						send(saveBurstDataMsg, "saveBurstData");
						_burstCounter++;
						_nofBurstCounter=0;
						_saveBurstData=true;
					}
				}
    		}
    	}
    } else {
        // finished
        delete msg;
    }
} // handleMessage()

void Source::sendPacket(WRPacket* packet) {

	// asynchronous arrival of new packets if packet sizes vary, else synchronous
	// reschedule the timer for the next message
	// consider interArrivalTime and ifg

	int prio = packet->getPriority();
	cGate* outputgate = getGate(prio);
	cDatarateChannel* channel = check_and_cast<cDatarateChannel *> (outputgate->getTransmissionChannel());
	simtime_t t = channel->calculateDuration(packet);

	if( (simTime()+t) <= simTime() || !channel->isBusy() ) {
		// channel free; send out packet immediately
		send(packet, outputgate);
		_sent.at(prio) = _sent.at(prio)+1;
		// trigger new packet generation
		// immediately send the next packet (full load)
		// cancel and re-schedule
		cancelEvent(startSendingPacket);
		scheduleAt((simTime()+t+_ifg), startSendingPacket);
		//cout << "simTime() " << simTime() << " next " << simTime()+t+_ifg << endl;

		// allow some time between two new packets (lower load)
		//scheduleAt((simTime()+t+_ifg+_interArrivalTime), startSendingPacket);
	}

	return;
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

