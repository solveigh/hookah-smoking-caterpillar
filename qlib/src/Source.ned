//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package qlib;

//
// A module that generates jobs. One can specify the number of jobs to be generated,
// the starting and ending time, and interval between generating jobs.
// Job generation stops when the number of jobs or the end time has been reached,
// whichever occurs first. The name, type and priority of jobs can be set as well.
//
simple Source
{
    parameters:
        @group(Queueing);
        @signal[created](type="int");
        @statistic[created](title="the number of jobs created";record=count;interpolationmode=none);
        @display("i=block/source");
        int numPackets = default(-1);               // number of jobs to be generated (-1 means no limit)
        volatile double interArrivalTime @unit(s); // time between generated jobs
		volatile double ifg @unit(s); 				// time for interframe gap
        double startTime @unit(s) = default(interArrivalTime); // when the module sends out the first packet
    
    	string inputDataFile = default("data_full_load.txt");
    	int nofCoS = default(8);	// default: 7..0 
    gates:
        output pppg[] @labels(PPPFrame-conn);
}

