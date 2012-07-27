//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "PassiveQueue.h"


namespace qlib {

Define_Module(PassiveQueue);

PassiveQueue::PassiveQueue()
{
}

PassiveQueue::~PassiveQueue()
{
}

void PassiveQueue::initialize()
{
    droppedSignal = registerSignal("dropped");
    queueingTimeSignal = registerSignal("queueingTime");
    queueLengthSignal = registerSignal("queueLength");
    emit(queueLengthSignal, 0);

    _capacity = par("capacity");
    _size = par("size");
    queue.setName("queue");

    fifo = par("fifo");

	router = (cModule*)getParentModule()->findObject("router", true);

    std::cout << this->getName() << endl;
} // initialize()

void PassiveQueue::finish()
{
    //std::cout << this->getName() <<  " " << queue.length() << endl;
}

void PassiveQueue::handleMessage(cMessage *msg)
{
	//std::cout << " " << __FILE__ << ": "<< __FUNCTION__ << " " << msg->getName() << std::endl;
	WRPacket *job = check_and_cast<WRPacket *>(msg);

	//cout << simTime() << " " << __FILE__ << ": " << msg->getName() << " queue size " << this->_size << " capacity " << this->_capacity << endl;
	enqueue(job);

    // change the icon color
    if (ev.isGUI())
        getDisplayString().setTagArg("i",1, queue.empty() ? "" : "cyan3");
} // handleMessage()

void PassiveQueue::enqueue(cPacket* msg) {
	WRPacket *job = check_and_cast<WRPacket *>(msg);
	job->setTimestamp();

	// Tail-Drop: Drop newly arriving packet
	// check for container capacity
	if ((_capacity >= 0) && (queue.length() >= _capacity) && (size()>_size)) {
		_dropped.push_back(job);
		EV << "Queue full! Packet dropped.\n";
		if (ev.isGUI())
			bubble("Dropped!");
		emit(droppedSignal, 1);
		delete msg;
		return;
	}

	job->setOperationCounter(1);
	// queue everything
	queue.insert(job);
	emit(queueLengthSignal, length());
	job->setQueueCount(job->getQueueCount() + 1);
	//std::cout << this->getName() << " Q size " << queue.length() << std::endl;
} // enequeue()

int PassiveQueue::length()
{
    return queue.length();
} // length()

int PassiveQueue::size()
{
	int size = 0;
	for( int i=0; i<length(); i++ ) {
		size += check_and_cast<WRPacket *>(queue.get(i))->getByteLength();
	}
    return size;
} // size()

void PassiveQueue::request(int gateIndex)
{
    Enter_Method("request()!");

    ASSERT(!queue.empty());
    WRPacket *job;
    if (fifo) {
        job = (WRPacket *)queue.pop();
    }
    else {
        job = (WRPacket *)queue.back();
        // FIXME this may have bad performance as remove uses linear search
        queue.remove(job);
    }
    emit(queueLengthSignal, length());

    job->setOperationCounter( job->getOperationCounter()+queue.length() );

    job->setQueueCount(job->getQueueCount()+1);
    simtime_t d = simTime() - job->getTimestamp();
    job->setTotalQueueingTime(job->getTotalQueueingTime() + d);
    //cout << "queuingTime " << job->getTotalQueueingTime() << endl;
    emit(queueingTimeSignal, d);

    // reset timestamp
    job->setTimestamp();

    //cout << simTime() << " " << __FILE__ << ": " << job->getName() << endl;
    send(job, "out"); //, gateIndex);

    if (ev.isGUI())
        getDisplayString().setTagArg("i",1, queue.empty() ? "" : "cyan");
} // request()

WRPacket * PassiveQueue::front() {
	return (WRPacket *)queue.front();
} // front()


}; //namespace

