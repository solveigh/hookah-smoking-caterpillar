//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Sink.h"
#include "WRPacket.h"

namespace qlib {

Define_Module(Sink);

inline double s2us(double s) {
	return (s * 1000000);	// x 10e6
}

inline double s2ns(double s) {
	return (s * 1000000000);	// x 10e9
}

double Sink::perc(int dropped, int sent) {
	double d=0.;
	if( sent >0 )
		d = (double(dropped)/double(sent))*100.0;
	return d;
}

void Sink::initialize() {
	lifeTimeSignal = registerSignal("lifeTime");
	totalQueueingTimeSignal = registerSignal("totalQueueingTime");
	queuesVisitedSignal = registerSignal("queuesVisited");
	totalServiceTimeSignal = registerSignal("totalServiceTime");
	totalDelayTimeSignal = registerSignal("totalDelayTime");
	delaysVisitedSignal = registerSignal("delaysVisited");
	generationSignal = registerSignal("generation");
	keepJobs = par("keepJobs");

	_nofCoS = par("nofCoS");
	cout << "Sink nofCoS " << _nofCoS << endl;

	_burstTest = par("burstTest");

	numReceived = 0;
	WATCH(numReceived);

	_source = dynamic_cast<Source *>((cModule*)getParentModule()->findObject("source", true));
	_scheduler = dynamic_cast<Scheduler *>((cModule*)getParentModule()->findObject("scheduler", true));

    // pointers to other queues
	for( int i=0; i<_nofCoS;i++ )
    	_qs.push_back( getQueue(i) );
}

Queue *Sink::getQueue(int index) {
	std::string queue = "queue";
	char buffer[3];

	sprintf(buffer,"%d",index);
	buffer[2]='\0';
	queue += buffer;
	cModule *module = getParentModule()->getSubmodule(queue.c_str());
	Queue *pqueue = dynamic_cast<Queue *>(module);

	return pqueue;
} // getQueue()

void Sink::handleMessage(cMessage *msg) {

	if( strcmp(msg->getName(),"saveBurstData")!=0 ) {
		WRPacket *packet = check_and_cast<WRPacket *>(msg);
		simtime_t lifetime = msg->getArrivalTime() - msg->getCreationTime();
		numReceived++;

		switch( packet->getPriority() ) {
		case 0:
			v0.push_back(lifetime);
			vq0.push_back(packet->getTotalQueueingTime());
			vs0.push_back(packet->getTotalServiceTime());
			vsch0.push_back(calculateSchedulingTime(msg->getCreationTime(), simTime(), packet->getTotalQueueingTime()));
			break;
		case 1:
			v1.push_back(lifetime);
			vq1.push_back(packet->getTotalQueueingTime());
			vs1.push_back(packet->getTotalServiceTime());
			vsch1.push_back(calculateSchedulingTime(msg->getCreationTime(), simTime(), packet->getTotalQueueingTime()));
			break;
		case 2:
			v2.push_back(lifetime);
			vq2.push_back(packet->getTotalQueueingTime());
			vs2.push_back(packet->getTotalServiceTime());
			vsch2.push_back(calculateSchedulingTime(msg->getCreationTime(), simTime(), packet->getTotalQueueingTime()));
			break;
		case 3:
			v3.push_back(lifetime);
			vq3.push_back(packet->getTotalQueueingTime());
			vs3.push_back(packet->getTotalServiceTime());
			vsch3.push_back(calculateSchedulingTime(msg->getCreationTime(), simTime(), packet->getTotalQueueingTime()));
			break;
		case 4:
			v4.push_back(lifetime);
			vq4.push_back(packet->getTotalQueueingTime());
			vs4.push_back(packet->getTotalServiceTime());
			vsch4.push_back(calculateSchedulingTime(msg->getCreationTime(), simTime(), packet->getTotalQueueingTime()));
			break;
		case 5:
			v5.push_back(lifetime);
			vq5.push_back(packet->getTotalQueueingTime());
			vs5.push_back(packet->getTotalServiceTime());
			vsch5.push_back(calculateSchedulingTime(msg->getCreationTime(), simTime(), packet->getTotalQueueingTime()));
			break;
		case 6:
			v6.push_back(lifetime);
			vq6.push_back(packet->getTotalQueueingTime());
			vs6.push_back(packet->getTotalServiceTime());
			vsch6.push_back(calculateSchedulingTime(msg->getCreationTime(), simTime(), packet->getTotalQueueingTime()));
			break;
		case 7:
			v7.push_back(lifetime);
			vq7.push_back(packet->getTotalQueueingTime());
			vs7.push_back(packet->getTotalServiceTime());
			vsch7.push_back(calculateSchedulingTime(msg->getCreationTime(), simTime(), packet->getTotalQueueingTime()));
			break;
		default:
			break;
		}
		cancelAndDelete(msg);
	} else {
		if( _burstTest==true ) {
			// write result files

			int burstCounter = _source->getBurstCounter();
			int nofBursts = Useful::getInstance()->getBurstIntervals().at(burstCounter);
			cout << "simtime " << simTime() << "burst interval of " << nofBursts << " now at " << burstCounter << endl;

			int nofCreated = _source->getCreated();
			int nofArrived=-1;
			if( _nofCoS==8 )
				nofArrived = v0.size()+v1.size()+v2.size()+v3.size()+v4.size()+v5.size()+v6.size()+v7.size();
			if( _nofCoS==3 )
				nofArrived = v0.size()+v1.size()+v2.size();

			cout << "nofCreated " << nofCreated << " nofArrived " << nofArrived << endl;

			determineQueueSizes();

			if( _source->saveBurstData()==true ) {
				cout << "burst ctr " << burstCounter << " # bursts " << nofBursts << endl;
				char buf[30];
				sprintf(buf, "burst_%d_%d_", nofBursts, nofCreated);
				string begin = string(buf);

				cout << " created: " << nofCreated  << " arrived: " << nofArrived << endl;
				cout << " End burst: " << simTime() << endl;

				// create comma separated text file for easy evaluation
				write2File("out.csv");

				string fname = begin + string("dropped_");
				fname+=_source->getInputDataFileName();
				fname+=string(".csv");

				writeDropped2File(fname);
				writeDropped2File4Plot(fname);
				writeDropped2File4PlotPercentage(fname);

				fname = begin + string("dropped_perc_");
				fname+=_source->getInputDataFileName();
				fname+=string(".csv");
				writeDropped2FilePercentage(fname);

				fname = begin + string("times_");
				fname+=_source->getInputDataFileName();
				fname+=string(".csv");
				writeQueuingSchedulingTimes2File(fname);

				writeTimes2File4Plot(fname);
				writeQueuingTimes2File4Plot(fname);

				fname = begin + string("times_");
				fname+=_scheduler->getSchedulingAlgorithm();
				fname+=string(".csv");
				writeQueuingSchedulingTimes2File4Table(fname);

				fname = begin + string("dropped_perc_");
				fname+=_scheduler->getSchedulingAlgorithm();
				fname+=string(".csv");
				writeDropped2FilePercentage4Table(fname);
			}
			cancelAndDelete(msg);
		} // if( _burstTest==true )
	}
	canFinish();
} // handleMessage()

void Sink::canFinish() {
	// finish simulation on two conditions:
	// 1) source has created all packets it was supposed to do
	// 2) no packets are left in queues
	vector<int> vCanFinish;
	if( _source->getCreated() == _source->getNumPackets() ) {
		for( int i=0; i<_nofCoS; i++ ) {
			if( getQueue(i)->length()!=0 ) {
				vCanFinish.push_back(0);
			} else {
				vCanFinish.push_back(1);
			}
		}

		vector<int>::iterator vit;
		int cnt=0;
		for( vit=vCanFinish.begin(); vit!=vCanFinish.end(); vit++ ) {
			if( *vit==1 ) {
				cnt++;
			}
		}
		if( (vCanFinish.size()>0) && (cnt==vCanFinish.size()) ) {
			endSimulation();
		}
	}
} // canFinish()

double Sink::calculatePacketLoss( int priorityQueue ) {
	double dropped=100.0;
	dropped = (double(_qs.at(priorityQueue)->getDropped().size())/double(_source->getSent().at(priorityQueue)))*100.0;

	int n = _qs.at(priorityQueue)->getDropped().size() + getQueue(priorityQueue)->length();
	if( n == _source->getSent().at(priorityQueue) ) { // queue was never emptied
		dropped = 100.0;
	}
	return dropped;
} //calculatePacketLoss()

void Sink::finish() {
	// overview
	cout << this->getName() << ": CoS: " << _qs.size() << " " << _scheduler->getSchedulingAlgorithm() << " Scenario: " << _source->getInputDataFileName() << endl;

	if(_nofCoS==8) {
		std::cout << "p 0: avg " << avg_lifetime(v0) << " ns size " << v0.size() << " Q time: " << avg_lifetime(vq0) << " ns "<</*S time: " << avg_lifetime(vsch0) << " ns*/ " sent: " << _source->getSent().at(0) << " dropped " << _qs.at(0)->getDropped().size() << "( " << calculatePacketLoss(0) << "%)" << std::endl;
		std::cout << "p 1: avg " << avg_lifetime(v1) << " ns size " << v1.size() << " Q time: " << avg_lifetime(vq1) << " ns "<</*S time: " << avg_lifetime(vsch1) << " ns*/ " sent: " << _source->getSent().at(1) << " dropped " << _qs.at(1)->getDropped().size() << "( " << (double(_qs.at(1)->getDropped().size())/double(_source->getSent().at(1)))*100.0 << "%)" << std::endl;
		std::cout << "p 2: avg " << avg_lifetime(v2) << " ns size " << v2.size() << " Q time: " << avg_lifetime(vq2) << " ns "<</*S time: " << avg_lifetime(vsch2) << " ns*/ " sent: " << _source->getSent().at(2) << " dropped " << _qs.at(2)->getDropped().size() << "( " << (double(_qs.at(2)->getDropped().size())/double(_source->getSent().at(2)))*100.0 << "%)" << std::endl;
		std::cout << "p 3: avg " << avg_lifetime(v3) << " ns size " << v3.size() << " Q time: " << avg_lifetime(vq3) << " ns "<</*S time: " << avg_lifetime(vsch3) << " ns*/ " sent: " << _source->getSent().at(3) << " dropped " << _qs.at(3)->getDropped().size() << "( " << (double(_qs.at(3)->getDropped().size())/double(_source->getSent().at(3)))*100.0 << "%)" << std::endl;
		std::cout << "p 4: avg " << avg_lifetime(v4) << " ns size " << v4.size() << " Q time: " << avg_lifetime(vq4) << " ns "<</*S time: " << avg_lifetime(vsch4) << " ns*/ " sent: " << _source->getSent().at(4) << " dropped " << _qs.at(4)->getDropped().size() << "( " << (double(_qs.at(4)->getDropped().size())/double(_source->getSent().at(4)))*100.0 << "%)" << std::endl;
		std::cout << "p 5: avg " << avg_lifetime(v5) << " ns size " << v5.size() << " Q time: " << avg_lifetime(vq5) << " ns "<</*S time: " << avg_lifetime(vsch5) << " ns*/ " sent: " << _source->getSent().at(5) << " dropped " << _qs.at(5)->getDropped().size() << "( " << (double(_qs.at(5)->getDropped().size())/double(_source->getSent().at(5)))*100.0 << "%)" << std::endl;
		std::cout << "p 6: avg " << avg_lifetime(v6) << " ns size " << v6.size() << " Q time: " << avg_lifetime(vq6) << " ns "<</*S time: " << avg_lifetime(vsch6) << " ns*/ " sent: " << _source->getSent().at(6) << " dropped " << _qs.at(6)->getDropped().size() << "( " << (double(_qs.at(6)->getDropped().size())/double(_source->getSent().at(6)))*100.0 << "%)" << std::endl;
		std::cout << "p 7: avg " << avg_lifetime(v7) << " ns size " << v7.size() << " Q time: " << avg_lifetime(vq7) << " ns "<</*S time: " << avg_lifetime(vsch7) << " ns*/ " sent: " << _source->getSent().at(7) << " dropped " << _qs.at(7)->getDropped().size() << "( " << (double(_qs.at(7)->getDropped().size())/double(_source->getSent().at(7)))*100.0 << "%)" << std::endl;
	} else if(_nofCoS==3) {
		std::cout << "p 0: avg " << avg_lifetime(v0) << " ns size " << v0.size() << " Q time: " << avg_lifetime(vq0) << " ns "<</*S time: " << avg_lifetime(vsch0) << " ns*/ " sent: " << _source->getSent().at(0) << " dropped " << _qs.at(0)->getDropped().size() << "( " << (double(_qs.at(0)->getDropped().size())/double(_source->getSent().at(0)))*100.0 << "%)" << std::endl;
		std::cout << "p 1: avg " << avg_lifetime(v1) << " ns size " << v1.size() << " Q time: " << avg_lifetime(vq1) << " ns "<</*S time: " << avg_lifetime(vsch1) << " ns*/ " sent: " << _source->getSent().at(1) << " dropped " << _qs.at(1)->getDropped().size() << "( " << (double(_qs.at(1)->getDropped().size())/double(_source->getSent().at(1)))*100.0 << "%)" << std::endl;
		std::cout << "p 2: avg " << avg_lifetime(v2) << " ns size " << v2.size() << " Q time: " << avg_lifetime(vq2) << " ns "<</*S time: " << avg_lifetime(vsch2) << " ns*/ " sent: " << _source->getSent().at(2) << " dropped " << _qs.at(2)->getDropped().size() << "( " << (double(_qs.at(2)->getDropped().size())/double(_source->getSent().at(2)))*100.0 << "%)" << std::endl;
	}
	determineQueueSizes();

	// find out number of objects still stuck in server

	int nofCreated = _source->getCreated();
	int nofArrived=-1;
	if( _nofCoS==8 )
		nofArrived = v0.size()+v1.size()+v2.size()+v3.size()+v4.size()+v5.size()+v6.size()+v7.size();
	if( _nofCoS==3 )
		nofArrived = v0.size()+v1.size()+v2.size();
	cout << " created: " << nofCreated  << " arrived: " << nofArrived << endl;
	cout << " End: " << simTime() << endl;

	// create comma separated text file for easy evaluation
	write2File("out.csv");

	string fname = string("dropped_");
	fname+=_source->getInputDataFileName();
	fname+=string(".csv");

	writeDropped2File(fname);
	writeDropped2File4Plot(fname);
	writeDropped2File4PlotPercentage(fname);

	fname = string("dropped_perc_");
	fname+=_source->getInputDataFileName();
	fname+=string(".csv");
	writeDropped2FilePercentage(fname);

	fname = string("times_");
	fname+=_source->getInputDataFileName();
	fname+=string(".csv");
	writeQueuingSchedulingTimes2File(fname);

	writeTimes2File4Plot(fname);
	writeQueuingTimes2File4Plot(fname);

	fname = string("times_");
	fname+=_scheduler->getSchedulingAlgorithm();
	fname+=string(".csv");
	writeQueuingSchedulingTimes2File4Table(fname);

	fname = string("dropped_perc_");
	fname+=_scheduler->getSchedulingAlgorithm();
	fname+=string(".csv");
	writeDropped2FilePercentage4Table(fname);

	writeNumbers2TexFile("numbers.tex");

	// remove all indisposed messages in the end!
	this->setPerformFinalGC(true);

#ifdef __linux__
	if( strcmp(_scheduler->getSchedulingAlgorithm().c_str(),"fcfs")==0 )
		file_delete("fcfsdata");
#endif
} // finish()

void Sink::writeTimes2File(string filename) {
	string str;
	char buf[250];

    if(_nofCoS==8) {
		sprintf(buf,"Priority,7 (%d), 6 (%d),5 (%d),4 (%d),3 (%d),2 (%d),1 (%d),0 (%d)", _source->getSent().at(7), _source->getSent().at(6), _source->getSent().at(5), _source->getSent().at(4), _source->getSent().at(3), _source->getSent().at(2), _source->getSent().at(1),_source->getSent().at(0));
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
		sprintf(buf,"%s,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf", _scheduler->getSchedulingAlgorithm().c_str(),avg_lifetime(vq7), avg_lifetime(vq6), avg_lifetime(vq5), avg_lifetime(vq4),
		avg_lifetime(vq3), avg_lifetime(vq2), avg_lifetime(vq1), avg_lifetime(vq0));
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
    } else if (_nofCoS==3) {
		sprintf(buf,"Priority,2 (%d),1 (%d),0 (%d)", _source->getSent().at(2), _source->getSent().at(1),_source->getSent().at(0));
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
		sprintf(buf,"%s,%2.0lf,%2.0lf,%2.0lf", _scheduler->getSchedulingAlgorithm().c_str(), avg_lifetime(vq2), avg_lifetime(vq1), avg_lifetime(vq0));
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
    }
} //writeTimes2File()

void Sink::writeQueuingSchedulingTimes2File(string filename) {
	string str;
	char buf[250];

    if(_nofCoS==8) {
    	if( !Useful::getInstance()->testFirstLineOfFile(filename, "Algorithm") ) {
			sprintf(buf,"Algorithm,7 (%d),7 (%d), 6 (%d),6 (%d), 5 (%d),5 (%d), 4 (%d),4 (%d), 3 (%d),3 (%d), 2 (%d),2 (%d), 1 (%d),1 (%d), 0 (%d),0 (%d)",
					_source->getSent().at(7), _source->getSent().at(7),
					_source->getSent().at(6),_source->getSent().at(6),
					_source->getSent().at(5),_source->getSent().at(5),
					_source->getSent().at(4),_source->getSent().at(4),
					_source->getSent().at(3),_source->getSent().at(3),
					_source->getSent().at(2),_source->getSent().at(2),
					_source->getSent().at(1),_source->getSent().at(1),
					_source->getSent().at(0),_source->getSent().at(0));
			str = string(buf);
			Useful::getInstance()->appendToFile(filename, str);
    	}
		sprintf(buf,"%s,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf", _scheduler->getSchedulingAlgorithm().c_str(),
				avg_lifetime(vq7),avg_lifetime(vsch7),
				avg_lifetime(vq6),avg_lifetime(vsch6),
				avg_lifetime(vq5),avg_lifetime(vsch5),
				avg_lifetime(vq4),avg_lifetime(vsch4),
				avg_lifetime(vq3),avg_lifetime(vsch3),
				avg_lifetime(vq2),avg_lifetime(vsch2),
				avg_lifetime(vq1),avg_lifetime(vsch1),
				avg_lifetime(vq0),avg_lifetime(vsch0));
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
    } else if (_nofCoS==3) {
    	if( !Useful::getInstance()->testFirstLineOfFile(filename, "Algorithm") ) {
			sprintf(buf,"Algorithm,2 (%d),2 (%d), 1 (%d),1 (%d), 0 (%d),0 (%d)",
					_source->getSent().at(2), _source->getSent().at(2),
					_source->getSent().at(1),_source->getSent().at(1),
					_source->getSent().at(0),_source->getSent().at(0));
			str = string(buf);
			Useful::getInstance()->appendToFile(filename, str);
    	}
		sprintf(buf,"%s,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf,%2.0lf", _scheduler->getSchedulingAlgorithm().c_str(),
				avg_lifetime(vq2),avg_lifetime(vsch2),
				avg_lifetime(vq1),avg_lifetime(vsch1),
				avg_lifetime(vq0),avg_lifetime(vsch0));
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
    }
} //writeQueuingSchedulingTimes2File()

void Sink::writeQueuingSchedulingTimes2File4Table(string filename) {
	string str;
	char buf[250];

    if(_nofCoS==8) {
    	if( !Useful::getInstance()->testFirstLineOfFile(filename, "Scenario") ) {
    		sprintf(buf,"Scenario,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf",
    				7,7.0,7.0,
    				6,6.0,6.0,
    				5,5.0,5.0,
    				4,4.0,4.0,
    				3,3.0,3.0,
    				2,0.0,2.0,
    				1,1.0,1.0,
    				0,0.0,0.0
    				);
			str = string(buf);
			Useful::getInstance()->appendToFile(filename, str);
    	}
		sprintf(buf,"%s,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf",
				_source->getInputDataFileName().c_str(),
				_source->getSent().at(7),avg_lifetime(vq7),avg_lifetime(vsch7),
				_source->getSent().at(6),avg_lifetime(vq6),avg_lifetime(vsch6),
				_source->getSent().at(5),avg_lifetime(vq5),avg_lifetime(vsch5),
				_source->getSent().at(4),avg_lifetime(vq4),avg_lifetime(vsch4),
				_source->getSent().at(3),avg_lifetime(vq3),avg_lifetime(vsch3),
				_source->getSent().at(2),avg_lifetime(vq2),avg_lifetime(vsch2),
				_source->getSent().at(1),avg_lifetime(vq1),avg_lifetime(vsch1),
				_source->getSent().at(0),avg_lifetime(vq0),avg_lifetime(vsch0));
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
    } else if (_nofCoS==3) {
    	if( !Useful::getInstance()->testFirstLineOfFile(filename, "Scenario") ) {
    		sprintf(buf,"Scenario,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf",
     				2,0.0,2.0,
    				1,1.0,1.0,
    				0,0.0,0.0
    				);
			str = string(buf);
			Useful::getInstance()->appendToFile(filename, str);
    	}
		sprintf(buf,"%s,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf,%d,%2.0lf,%2.0lf", _source->getInputDataFileName().c_str(),
				_source->getSent().at(2),avg_lifetime(vq2),avg_lifetime(vsch2),
				_source->getSent().at(1),avg_lifetime(vq1),avg_lifetime(vsch1),
				_source->getSent().at(0),avg_lifetime(vq0),avg_lifetime(vsch0));
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
    }
} //writeQueuingSchedulingTimes2File4Table()

void Sink::writeQueuingTimes2File4Plot(string filename) {
	string fname = string("plot_queuing_");
	fname+=_scheduler->getSchedulingAlgorithm();
	fname+="_";
	fname += filename;
	string str;
	char buf[20];
	if( _nofCoS==8 ) {
		sprintf(buf,"7 %2.0lf", avg_lifetime(vq7));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"6 %2.0lf", avg_lifetime(vq6));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"5 %2.0lf", avg_lifetime(vq5));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"4 %2.0lf", avg_lifetime(vq4));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"3 %2.0lf", avg_lifetime(vq3));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
	}
	sprintf(buf,"2 %2.0lf",	avg_lifetime(vq2));
	str = string(buf);
	Useful::getInstance()->appendToFile(fname, str);
	sprintf(buf,"1 %2.0lf", avg_lifetime(vq1));
	str = string(buf);
	Useful::getInstance()->appendToFile(fname, str);
	sprintf(buf,"0 %2.0lf", avg_lifetime(vq0));
	str = string(buf);
	Useful::getInstance()->appendToFile(fname, str);
} //writeQueuingTimes2File4Plot()

void Sink::writeTimes2File4Plot(string filename) {
	string fname = string("plot_");
	fname+=_scheduler->getSchedulingAlgorithm();
	fname+="_";
	fname += filename;
	string str;
	char buf[20];
	if( _nofCoS==8 ) {
		sprintf(buf,"7 %2.0lf", avg_lifetime(vsch7));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"6 %2.0lf", avg_lifetime(vsch6));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"5 %2.0lf", avg_lifetime(vsch5));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"4 %2.0lf", avg_lifetime(vsch4));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"3 %2.0lf", avg_lifetime(vsch3));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
	}
	sprintf(buf,"2 %2.0lf",	avg_lifetime(vsch2));
	str = string(buf);
	Useful::getInstance()->appendToFile(fname, str);
	sprintf(buf,"1 %2.0lf", avg_lifetime(vsch1));
	str = string(buf);
	Useful::getInstance()->appendToFile(fname, str);
	sprintf(buf,"0 %2.0lf", avg_lifetime(vsch0));
	str = string(buf);
	Useful::getInstance()->appendToFile(fname, str);
} //writeTimes2File4Plot()

void Sink::writeDropped2File(string filename) {
	string str;
	char buf[150];
	if( _nofCoS==8 ) {
#ifdef __linux__
		if( !Useful::getInstance()->testFirstLineOfFile(filename, "Algorithm") ) {
			sprintf(buf,"Algorithm,7 (%d),6 (%d),5 (%d),4 (%d),3 (%d),2 (%d),1 (%d),0 (%d)", _source->getSent().at(7), _source->getSent().at(6), _source->getSent().at(5),
					_source->getSent().at(4),_source->getSent().at(3), _source->getSent().at(2), _source->getSent().at(1), _source->getSent().at(0));
			str = string(buf);
			Useful::getInstance()->appendToFile(filename, str);
		}
#endif
		sprintf(buf,"%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", _scheduler->getSchedulingAlgorithm().c_str(), _source->getSent().at(7), _qs.at(7)->getDropped().size(),
				_source->getSent().at(6), _qs.at(6)->getDropped().size(), _source->getSent().at(5), _qs.at(5)->getDropped().size(),
				_source->getSent().at(4), _qs.at(4)->getDropped().size(), _source->getSent().at(3), _qs.at(3)->getDropped().size(),
				_source->getSent().at(2), _qs.at(2)->getDropped().size(), _source->getSent().at(1), _qs.at(1)->getDropped().size(),
				_source->getSent().at(0), _qs.at(0)->getDropped().size());
	} else if(_nofCoS==3) {
#ifdef __linux__
		if( !Useful::getInstance()->testFirstLineOfFile(filename, "Algorithm") ) {
			sprintf(buf,"Algorithm,2 (%d),1 (%d),0 (%d)", _source->getSent().at(2), _source->getSent().at(1), _source->getSent().at(0));
			str = string(buf);
			Useful::getInstance()->appendToFile(filename, str);
		}
#endif
		sprintf(buf,"%s,%d,%d,%d,%d,%d,%d", _scheduler->getSchedulingAlgorithm().c_str(),
				_source->getSent().at(2), _qs.at(2)->getDropped().size(),
				_source->getSent().at(1), _qs.at(1)->getDropped().size(),
				_source->getSent().at(0), _qs.at(0)->getDropped().size());
	}
	str = string(buf);
	Useful::getInstance()->appendToFile(filename, str);
} //writeDropped2File()

void Sink::writeDropped2File4Plot(string filename) {
	string fname = string("plot_");
	fname+=_scheduler->getSchedulingAlgorithm();
	fname+="_";
	fname += filename;
	string str;
	char buf[20];
	if( _nofCoS==8 ) {
		sprintf(buf,"7 %d", _qs.at(7)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"6 %d", _qs.at(6)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"5 %d", _qs.at(5)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"4 %d", _qs.at(4)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"3 %d", _qs.at(3)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
	}
	sprintf(buf,"2 %d",	_qs.at(2)->getDropped().size());
	str = string(buf);
	Useful::getInstance()->appendToFile(fname, str);
	sprintf(buf,"1 %d", _qs.at(1)->getDropped().size());
	str = string(buf);
	Useful::getInstance()->appendToFile(fname, str);
	sprintf(buf,"0 %d", _qs.at(0)->getDropped().size());
	str = string(buf);
	Useful::getInstance()->appendToFile(fname, str);
} //writeDropped2File4Plot()

void Sink::writeDropped2File4PlotPercentage(string filename) {
	string fname = string("plot_perc_");
	fname+=_scheduler->getSchedulingAlgorithm();
	fname+="_";
	fname += filename;
	string str;
	char buf[20];
	if( _nofCoS==8 ) {
		sprintf(buf,"7 %2.1lf", perc(_qs.at(7)->getDropped().size(),_source->getSent().at(7)));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"6 %2.1lf", perc(_qs.at(6)->getDropped().size(),_source->getSent().at(6)));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"5 %2.1lf", perc(_qs.at(5)->getDropped().size(),_source->getSent().at(5)));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"4 %2.1lf", perc(_qs.at(4)->getDropped().size(),_source->getSent().at(4)));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
		sprintf(buf,"3 %2.1lf", perc(_qs.at(3)->getDropped().size(),_source->getSent().at(3)));
		str = string(buf);
		Useful::getInstance()->appendToFile(fname, str);
	}
	sprintf(buf,"2 %2.1lf", perc(_qs.at(2)->getDropped().size(),_source->getSent().at(2)));
	str = string(buf);
	Useful::getInstance()->appendToFile(fname, str);
	sprintf(buf,"1 %2.1lf", perc(_qs.at(1)->getDropped().size(),_source->getSent().at(1)));
	str = string(buf);
	Useful::getInstance()->appendToFile(fname, str);
	sprintf(buf,"0 %2.1lf", perc(_qs.at(0)->getDropped().size(),_source->getSent().at(0)));
	str = string(buf);
	Useful::getInstance()->appendToFile(fname, str);
} //writeDropped2File4PlotPercentage()


void Sink::writeDropped2FilePercentage(string filename) {
	string str;
	char buf[150];
	if( _nofCoS==8 ) {
		if( !Useful::getInstance()->testFirstLineOfFile(filename, "Algorithm") ) {
			sprintf(buf,"Algorithm,7,7.0,6,6.0,5,5.0,4,4.0,3,3.0,2,2.0,1,1.0,0,0.0");
			str = string(buf);
			Useful::getInstance()->appendToFile(filename, str);
		}
		double d0=0., d1=0., d2=0., d3=0., d4=0., d5=0., d6=0., d7=0.;
		//cout << "dropped " << _qs.at(7)->getDropped().size() << " sent " << _source->getSent().at(7) << endl;
		d7 = perc(_qs.at(7)->getDropped().size(),_source->getSent().at(7));
		d6 = perc(_qs.at(6)->getDropped().size(),_source->getSent().at(6));
		d5 = perc(_qs.at(5)->getDropped().size(),_source->getSent().at(5));
		d4 = perc(_qs.at(4)->getDropped().size(),_source->getSent().at(4));
		d3 = perc(_qs.at(3)->getDropped().size(),_source->getSent().at(3));
		d2 = perc(_qs.at(2)->getDropped().size(),_source->getSent().at(2));
		d1 = perc(_qs.at(1)->getDropped().size(),_source->getSent().at(1));
		d0 = perc(_qs.at(0)->getDropped().size(),_source->getSent().at(0));
		sprintf(buf,"%s,%d,%2.1lf,%d,%2.1lf,%d,%2.1lf,%d,%2.1lf,%d,%2.1lf,%d,%2.1lf,%d,%2.1lf,%d,%2.1lf", _scheduler->getSchedulingAlgorithm().c_str(),  _qs.at(7)->getDropped().size(), d7,
				_qs.at(6)->getDropped().size(), d6, _qs.at(5)->getDropped().size(), d5, _qs.at(4)->getDropped().size(), d4, _qs.at(3)->getDropped().size(), d3,
				_qs.at(2)->getDropped().size(), d2, _qs.at(1)->getDropped().size(), d1, _qs.at(0)->getDropped().size(), d0);
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
	} else if(_nofCoS==3) {
		if( !Useful::getInstance()->testFirstLineOfFile(filename, "Algorithm") ) {
			sprintf(buf,"Algorithm,2,2.0,1,1.0,0,0.0");
			str = string(buf);
			Useful::getInstance()->appendToFile(filename, str);
		}
		double d0=0., d1=0., d2=0.;
		d2 = perc(_qs.at(2)->getDropped().size(),_source->getSent().at(2));
		d1 = perc(_qs.at(1)->getDropped().size(),_source->getSent().at(1));
		d0 = perc(_qs.at(0)->getDropped().size(),_source->getSent().at(0));
		sprintf(buf,"%s,%d,%2.1lf,%d,%2.1lf,%d,%2.1lf", _scheduler->getSchedulingAlgorithm().c_str(),
				_qs.at(2)->getDropped().size(), d2, _qs.at(1)->getDropped().size(), d1, _qs.at(0)->getDropped().size(), d0);
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
	}
} //writeDropped2FilePercentage()

void Sink::writeDropped2FilePercentage4Table(string filename) {
	string str;
	char buf[200];
	if( _nofCoS==8 ) {
    	if( !Useful::getInstance()->testFirstLineOfFile(filename, "Scenario") ) {
    		sprintf(buf,"Scenario,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf",
    				7,7,7.0,
    				6,6,6.0,
    				5,5,5.0,
    				4,4,4.0,
    				3,3,3.0,
    				2,0,2.0,
    				1,1,1.0,
    				0,0,0.0
    				);
			str = string(buf);
			Useful::getInstance()->appendToFile(filename, str);
    	}
		double d0=0., d1=0., d2=0., d3=0., d4=0., d5=0., d6=0., d7=0.;
		d7 = perc(_qs.at(7)->getDropped().size(),_source->getSent().at(7));
		d6 = perc(_qs.at(6)->getDropped().size(),_source->getSent().at(6));
		d5 = perc(_qs.at(5)->getDropped().size(),_source->getSent().at(5));
		d4 = perc(_qs.at(4)->getDropped().size(),_source->getSent().at(4));
		d3 = perc(_qs.at(3)->getDropped().size(),_source->getSent().at(3));
		d2 = perc(_qs.at(2)->getDropped().size(),_source->getSent().at(2));
		d1 = perc(_qs.at(1)->getDropped().size(),_source->getSent().at(1));
		d0 = perc(_qs.at(0)->getDropped().size(),_source->getSent().at(0));
		sprintf(buf,"%s,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf",
				_source->getInputDataFileName().c_str(),
				_source->getSent().at(7),_qs.at(7)->getDropped().size(), d7,
				_source->getSent().at(6),_qs.at(6)->getDropped().size(), d6,
				_source->getSent().at(5),_qs.at(5)->getDropped().size(), d5,
				_source->getSent().at(4),_qs.at(4)->getDropped().size(), d4,
				_source->getSent().at(3),_qs.at(3)->getDropped().size(), d3,
				_source->getSent().at(2),_qs.at(2)->getDropped().size(), d2,
				_source->getSent().at(1),_qs.at(1)->getDropped().size(), d1,
				_source->getSent().at(0),_qs.at(0)->getDropped().size(), d0);
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
	} else if(_nofCoS==3) {
    	if( !Useful::getInstance()->testFirstLineOfFile(filename, "Scenario") ) {
    		sprintf(buf,"Scenario,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf",
    				2,0,2.0,
    				1,1,1.0,
    				0,0,0.0
    				);
			str = string(buf);
			Useful::getInstance()->appendToFile(filename, str);
    	}
		double d0=0., d1=0., d2=0.;
		d2 = perc(_qs.at(2)->getDropped().size(),_source->getSent().at(2));
		d1 = perc(_qs.at(1)->getDropped().size(),_source->getSent().at(1));
		d0 = perc(_qs.at(0)->getDropped().size(),_source->getSent().at(0));
		sprintf(buf,"%s,%d,%d,%2.2lf,%d,%d,%2.2lf,%d,%d,%2.2lf", _source->getInputDataFileName().c_str(),
				_source->getSent().at(2), _qs.at(2)->getDropped().size(), d2,
				_source->getSent().at(1), _qs.at(1)->getDropped().size(), d1,
				_source->getSent().at(0), _qs.at(0)->getDropped().size(), d0);
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
	}
} //writeDropped2FilePercentage4Table()

void Sink::write2File(string filename) {
	string str;
	char buf[50];

	// create comma separated text file for easy evaluation
	Useful::getInstance()->appendToFile(filename, this->getName() );
	Useful::getInstance()->appendToFile(filename, _scheduler->getSchedulingAlgorithm());
	Useful::getInstance()->appendToFile(filename, _source->getInputDataFileName());

	if( _nofCoS==8 ) {
		sprintf(buf,"Lifetime,\tQ-Time,\tS-Time,\tSent,\tDropped", avg_lifetime(v7), avg_lifetime(vq7), avg_lifetime(vs7), _source->getSent().at(7), _qs.at(7)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
		sprintf(buf,"%2.0lf,\t%2.0lf,\t%2.0lf,\t%d,\t%d", avg_lifetime(v7), avg_lifetime(vq7), avg_lifetime(vs7), _source->getSent().at(7), _qs.at(7)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
		sprintf(buf,"%2.0lf,\t%2.0lf,\t%2.0lf,\t%d,\t%d", avg_lifetime(v6), avg_lifetime(vq6), avg_lifetime(vs6), _source->getSent().at(6), _qs.at(6)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
		sprintf(buf, "%2.0lf,\t%2.0lf,\t%2.0lf,\t%d,\t%d", avg_lifetime(v5), avg_lifetime(vq5), avg_lifetime(vs5), _source->getSent().at(5), _qs.at(5)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
		sprintf(buf, "%2.0lf,\t%2.0lf,\t%2.0lf,\t%d,\t%d", avg_lifetime(v4), avg_lifetime(vq4), avg_lifetime(vs4), _source->getSent().at(4), _qs.at(4)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
		sprintf(buf, "%2.0lf,\t%2.0lf,\t%2.0lf,\t%d,\t%d", avg_lifetime(v3), avg_lifetime(vq3), avg_lifetime(vs3), _source->getSent().at(3), _qs.at(3)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);

		sprintf(buf, "%2.0lf,\t%2.0lf,\t%2.0lf,\t%d,\t%d", avg_lifetime(v2), avg_lifetime(vq2), avg_lifetime(vs2), _source->getSent().at(2), _qs.at(2)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
		sprintf(buf, "%2.0lf,\t%2.0lf,\t%2.0lf,\t%d,\t%d", avg_lifetime(v1), avg_lifetime(vq1), avg_lifetime(vs1), _source->getSent().at(1), _qs.at(1)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
		sprintf(buf, "%2.0lf,\t%2.0lf,\t%2.0lf,\t%d,\t%d", avg_lifetime(v0), avg_lifetime(vq0), avg_lifetime(vs0), _source->getSent().at(0), _qs.at(0)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
	} else if (_nofCoS==3) {
		sprintf(buf,"Lifetime,\tQ-Time,\tS-Time,\tSent,\tDropped", avg_lifetime(v2), avg_lifetime(vq2), avg_lifetime(vs2), _source->getSent().at(2), _qs.at(2)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
		sprintf(buf, "%2.0lf,\t%2.0lf,\t%2.0lf,\t%d,\t%d", avg_lifetime(v2), avg_lifetime(vq2), avg_lifetime(vs2), _source->getSent().at(2), _qs.at(2)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
		sprintf(buf, "%2.0lf,\t%2.0lf,\t%2.0lf,\t%d,\t%d", avg_lifetime(v1), avg_lifetime(vq1), avg_lifetime(vs1), _source->getSent().at(1), _qs.at(1)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
		sprintf(buf, "%2.0lf,\t%2.0lf,\t%2.0lf,\t%d,\t%d", avg_lifetime(v0), avg_lifetime(vq0), avg_lifetime(vs0), _source->getSent().at(0), _qs.at(0)->getDropped().size());
		str = string(buf);
		Useful::getInstance()->appendToFile(filename, str);
	}
	int nofCreated = _source->getCreated();
	int nofArrived=-1;
	if( _nofCoS==8 )
		nofArrived = v0.size()+v1.size()+v2.size()+v3.size()+v4.size()+v5.size()+v6.size()+v7.size();
	if( _nofCoS==3 )
		nofArrived = v0.size()+v1.size()+v2.size();

	sprintf(buf, "created: %d / arrived: %d", nofCreated, nofArrived);
	str = string(buf);
	Useful::getInstance()->appendToFile(filename, str);
} // write2File()

double Sink::avg_lifetime(vector<double> v) {
	if( v.size()>0 ) {
		double avg_lt = 0.;
		std::vector<double>::iterator lit;
		for (lit = v.begin(); lit != v.end(); lit++) {
			avg_lt += (*lit);
		}
		avg_lt /= v.size();
		return s2ns((double) avg_lt);
	} else {
		return 0.0;
	}
} // avg_lifetime()

double Sink::avg_lifetime(vector<simtime_t> v) {
	if( v.size()>0 ) {
		double avg_lt = 0.;
		std::vector<simtime_t>::iterator lit;
		for (lit = v.begin(); lit != v.end(); lit++) {
			avg_lt += (*lit).dbl();
		}
		avg_lt /= v.size();
		return s2ns((double) avg_lt);
	} else {
		return 0.0;
	}
} // avg_lifetime()

void Sink::determineQueueSizes() {

	std::string queue;
	char buffer[3];

	for (int i = 0; i < _nofCoS; i++) {
		queue = "queue";
		sprintf(buffer, "%d", i);
		buffer[2] = '\0';
		queue += buffer;
		cModule *module = getParentModule()->getSubmodule(queue.c_str());
		IPassiveQueue *pqueue = dynamic_cast<IPassiveQueue *>(module);
		if (pqueue != NULL)
			std::cout << queue.c_str() << " length " << pqueue->length()
					<< std::endl;
	}
} // determineQueueSizes()

simtime_t Sink::calculateSchedulingTime(simtime_t source, simtime_t sink, simtime_t queue) {
	simtime_t result;
	result = (sink-source)-queue;
	return result;
} // calculateSchedulingTime()


void Sink::writeNumbers2TexFile(string filename) {

	string str;
	char buf[20];
	if( _nofCoS==8 ) {
		// most simple approach
		// HP max 75 %
		if( strcmp(_source->getInputDataFileName().c_str(), "K8HPHP75maxSPmin")==0 ) {
			str += "%<*NumPacketsHP75maxSPmin7>";
			sprintf(buf,"%d", _source->getSent().at(7));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmin7>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmin6>";
			sprintf(buf,"%d", _source->getSent().at(6));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmin6>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmin5>";
			sprintf(buf,"%d", _source->getSent().at(5));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmin5>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmin4>";
			sprintf(buf,"%d", _source->getSent().at(4));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmin4>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmin3>";
			sprintf(buf,"%d", _source->getSent().at(3));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmin3>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmin2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmin2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmin1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmin1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmin0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmin0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K8HP75maxSPmax")==0 ) {
			str += "%<*NumPacketsHP75maxSPmax7>";
			sprintf(buf,"%d",_source->getSent().at(7));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmax7>";
			Useful::getInstance()->appendToFile(filename, str);
			str="";
			str += "%<*NumPacketsHP75maxSPmax6>";
			sprintf(buf,"%d", _source->getSent().at(6));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmax6>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmax5>";
			sprintf(buf,"%d", _source->getSent().at(5));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmax5>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmax4>";
			sprintf(buf,"%d", _source->getSent().at(4));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmax4>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmax3>";
			sprintf(buf,"%d", _source->getSent().at(3));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmax3>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmax2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmax2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmax1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmax1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmax0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmax0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K8HP75maxSPvar")==0 ) {
			str += "%<*NumPacketsHP75maxSPvar7>";
			sprintf(buf,"%d",_source->getSent().at(7));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPvar7>";
			Useful::getInstance()->appendToFile(filename, str);
			str="";
			str += "%<*NumPacketsHP75maxSPvar6>";
			sprintf(buf,"%d", _source->getSent().at(6));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPvar6>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPvar5>";
			sprintf(buf,"%d", _source->getSent().at(5));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPvar5>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPvar4>";
			sprintf(buf,"%d", _source->getSent().at(4));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPvar4>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPvar3>";
			sprintf(buf,"%d", _source->getSent().at(3));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPvar3>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPvar2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPvar2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPvar1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPvar1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPvar0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPvar0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		// HP max 25%
		if( strcmp(_source->getInputDataFileName().c_str(), "K8HP25maxSPmin")==0 ) {
			str += "%<*NumPacketsHP25maxSPmin7>";
			sprintf(buf,"%d", _source->getSent().at(7));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmin7>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmin6>";
			sprintf(buf,"%d", _source->getSent().at(6));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmin6>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmin5>";
			sprintf(buf,"%d", _source->getSent().at(5));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmin5>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmin4>";
			sprintf(buf,"%d", _source->getSent().at(4));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmin4>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmin3>";
			sprintf(buf,"%d", _source->getSent().at(3));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmin3>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmin2>";
			buf[19] = '\0';
			str += buf;
			sprintf(buf,"%d", _source->getSent().at(2));
			str += "%</NumPacketsHP25maxSPmin2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmin1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmin1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmin0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmin0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K8HP25maxSPmax")==0 ) {
			str += "%<*NumPacketsHP25maxSPmax7>";
			sprintf(buf,"%d",_source->getSent().at(7));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmax7>";
			Useful::getInstance()->appendToFile(filename, str);
			str="";
			str += "%<*NumPacketsHP25maxSPmax6>";
			sprintf(buf,"%d", _source->getSent().at(6));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmax6>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmax5>";
			sprintf(buf,"%d", _source->getSent().at(5));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmax5>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmax4>";
			sprintf(buf,"%d", _source->getSent().at(4));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmax4>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmax3>";
			sprintf(buf,"%d", _source->getSent().at(3));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmax3>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmax2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmax2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmax1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmax1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmax0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmax0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K8HP25maxSPvar")==0 ) {
			str += "%<*NumPacketsHP25maxSPvar7>";
			sprintf(buf,"%d",_source->getSent().at(7));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPvar7>";
			Useful::getInstance()->appendToFile(filename, str);
			str="";
			str += "%<*NumPacketsHP25maxSPvar6>";
			sprintf(buf,"%d", _source->getSent().at(6));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPvar6>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPvar5>";
			sprintf(buf,"%d", _source->getSent().at(5));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPvar5>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPvar4>";
			sprintf(buf,"%d", _source->getSent().at(4));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPvar4>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPvar3>";
			sprintf(buf,"%d", _source->getSent().at(3));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPvar3>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPvar2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPvar2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPvar1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPvar1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPvar0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPvar0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}

		//"K8HP75minSPmin", "K8HP75minSPmax", "K8HP75minSPvar", "K8HP25minSPmin", "K8HP25minSPmax", "K8HP25minSPvar"
		// HP min 75 %
		if( strcmp(_source->getInputDataFileName().c_str(), "K8HP75minSPmin")==0 ) {
			str += "%<*NumPacketsHP75minSPmin7>";
			sprintf(buf,"%d", _source->getSent().at(7));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmin7>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmin6>";
			sprintf(buf,"%d", _source->getSent().at(6));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmin6>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmin5>";
			sprintf(buf,"%d", _source->getSent().at(5));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmin5>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmin4>";
			buf[19] = '\0';
			str += buf;
			sprintf(buf,"%d", _source->getSent().at(4));
			str += "%</NumPacketsHP75minSPmin4>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmin3>";
			buf[19] = '\0';
			str += buf;
			sprintf(buf,"%d", _source->getSent().at(3));
			str += "%</NumPacketsHP75minSPmin3>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmin2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmin2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmin1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmin1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmin0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmin0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K8HP75minSPmax")==0 ) {
			str += "%<*NumPacketsHP75minSPmax7>";
			sprintf(buf,"%d",_source->getSent().at(7));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmax7>";
			Useful::getInstance()->appendToFile(filename, str);
			str="";
			str += "%<*NumPacketsHP75minSPmax6>";
			sprintf(buf,"%d", _source->getSent().at(6));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmax6>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmax5>";
			sprintf(buf,"%d", _source->getSent().at(5));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmax5>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmax4>";
			sprintf(buf,"%d", _source->getSent().at(4));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmax4>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmax3>";
			sprintf(buf,"%d", _source->getSent().at(3));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmax3>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmax2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmax2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmax1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmax1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmax0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmax0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K8HP75minSPvar")==0 ) {
			str += "%<*NumPacketsHP75minSPvar7>";
			sprintf(buf,"%d",_source->getSent().at(7));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPvar7>";
			Useful::getInstance()->appendToFile(filename, str);
			str="";
			str += "%<*NumPacketsHP75minSPvar6>";
			sprintf(buf,"%d", _source->getSent().at(6));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPvar6>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPvar5>";
			sprintf(buf,"%d", _source->getSent().at(5));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPvar5>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPvar4>";
			sprintf(buf,"%d", _source->getSent().at(4));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPvar4>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPvar3>";
			sprintf(buf,"%d", _source->getSent().at(3));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPvar3>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPvar2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPvar2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPvar1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPvar1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPvar0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPvar0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		// HP min 25%
		if( strcmp(_source->getInputDataFileName().c_str(), "K8HP25minSPmin")==0 ) {
			str += "%<*NumPacketsHP25minSPmin7>";
			sprintf(buf,"%d", _source->getSent().at(7));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmin7>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmin6>";
			sprintf(buf,"%d", _source->getSent().at(6));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmin6>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmin5>";
			sprintf(buf,"%d", _source->getSent().at(5));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmin5>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmin4>";
			sprintf(buf,"%d", _source->getSent().at(4));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmin4>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmin3>";
			sprintf(buf,"%d", _source->getSent().at(3));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmin3>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmin2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmin2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmin1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmin1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmin0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmin0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K8HP25minSPmax")==0 ) {
			str += "%<*NumPacketsHP25minSPmax7>";
			sprintf(buf,"%d",_source->getSent().at(7));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmax7>";
			Useful::getInstance()->appendToFile(filename, str);
			str="";
			str += "%<*NumPacketsHP25minSPmax6>";
			sprintf(buf,"%d", _source->getSent().at(6));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmax6>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmax5>";
			sprintf(buf,"%d", _source->getSent().at(5));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmax5>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmax4>";
			sprintf(buf,"%d", _source->getSent().at(4));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmax4>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmax3>";
			sprintf(buf,"%d", _source->getSent().at(3));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmax3>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmax2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmax2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmax1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmax1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmax0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmax0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K8HP25minSPvar")==0 ) {
			str += "%<*NumPacketsHP25minSPvar7>";
			sprintf(buf,"%d",_source->getSent().at(7));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPvar7>";
			Useful::getInstance()->appendToFile(filename, str);
			str="";
			str += "%<*NumPacketsHP25minSPvar6>";
			sprintf(buf,"%d", _source->getSent().at(6));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPvar6>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPvar5>";
			sprintf(buf,"%d", _source->getSent().at(5));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPvar5>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPvar4>";
			sprintf(buf,"%d", _source->getSent().at(4));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPvar4>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPvar3>";
			sprintf(buf,"%d", _source->getSent().at(3));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPvar3>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPvar2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPvar2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPvar1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPvar1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPvar0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPvar0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
	} else if( _nofCoS==3 ) {	// TODO
		// most simple approach
		// HP max 75 %
		if( strcmp(_source->getInputDataFileName().c_str(), "K3HP75maxSPmin")==0 ) {
			str = "";
			str += "%<*NumPacketsHP75maxSPmin2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmin2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmin1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmin1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmin0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmin0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K3HP75maxSPmax")==0 ) {
			str = "";
			str += "%<*NumPacketsHP75maxSPmax2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmax2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmax1>";
			buf[19] = '\0';
			str += buf;
			sprintf(buf,"%d", _source->getSent().at(1));
			str += "%</NumPacketsHP75maxSPmax1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPmax0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPmax0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K3HP75maxSPvar")==0 ) {
			str = "";
			str += "%<*NumPacketsHP75maxSPvar2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPvar2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPvar1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPvar1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75maxSPvar0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75maxSPvar0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		// HP max 25%
		if( strcmp(_source->getInputDataFileName().c_str(), "K3HP25maxSPmin")==0 ) {
			str = "";
			str += "%<*NumPacketsHP25maxSPmin2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			sprintf(buf,"%d", _source->getSent().at(2));
			str += "%</NumPacketsHP25maxSPmin2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmin1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmin1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmin0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmin0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K3HP25maxSPmax")==0 ) {
			str = "";
			str += "%<*NumPacketsHP25maxSPmax2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmax2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmax1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmax1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPmax0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPmax0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K3HP25maxSPvar")==0 ) {
			str = "";
			str += "%<*NumPacketsHP25maxSPvar2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPvar2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPvar1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPvar1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25maxSPvar0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25maxSPvar0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}

		//"K3HP75minSPmin", "K3HP75minSPmax", "K3HP75minSPvar", "K3HP25minSPmin", "K3HP25minSPmax", "K3HP25minSPvar"
		// HP min 75 %
		if( strcmp(_source->getInputDataFileName().c_str(), "K3HP75minSPmin")==0 ) {
			str = "";
			str += "%<*NumPacketsHP75minSPmin2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmin2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmin1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmin1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmin0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmin0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K3HP75minSPmax")==0 ) {
			str = "";
			str += "%<*NumPacketsHP75minSPmax2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmax2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmax1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmax1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPmax0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPmax0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K3HP75minSPvar")==0 ) {
			str = "";
			str += "%<*NumPacketsHP75minSPvar2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPvar2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPvar1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPvar1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP75minSPvar0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP75minSPvar0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		// HP min 25%
		if( strcmp(_source->getInputDataFileName().c_str(), "K3HP25minSPmin")==0 ) {
			str = "";
			str += "%<*NumPacketsHP25minSPmin2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmin2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmin1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmin1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmin0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmin0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K3HP25minSPmax")==0 ) {
			str = "";
			str += "%<*NumPacketsHP25minSPmax2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmax2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmax1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmax1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPmax0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPmax0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
		if (strcmp(_source->getInputDataFileName().c_str(), "K3HP25minSPvar")==0 ) {
			str = "";
			str += "%<*NumPacketsHP25minSPvar2>";
			sprintf(buf,"%d", _source->getSent().at(2));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPvar2>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPvar1>";
			sprintf(buf,"%d", _source->getSent().at(1));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPvar1>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			str += "%<*NumPacketsHP25minSPvar0>";
			sprintf(buf,"%d", _source->getSent().at(0));
			buf[19] = '\0';
			str += buf;
			str += "%</NumPacketsHP25minSPvar0>";
			Useful::getInstance()->appendToFile(filename, str);
			str = "";
			return;
		}
	}
} // writeNumbers2TexFile()


}
;
//namespace

