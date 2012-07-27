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
// Sends the messages to different outputs depending on a set algorithm.
//
// @author rhornig
// @todo minDelay not implemented
//
simple Router
{
    parameters:
        @group(Queueing);
        @display("i=block/routing");
        string routingAlgorithm @enum("FCFS", "RR", "WRR", "SQF", "LQF", "WFQ", "FQSW") = default("Priority");
        volatile int randomGateIndex = default(intuniform(0, sizeof(out)-1));    // the destination gate in case of random routing
        
        volatile double serviceTime @unit(s); // service time of a job
    	int nofCoS = default(8);	// default: 7..0
    	
    	// N=8
    	int weight7 = default(4);	// weight used in WFQ
    	int weight6 = default(3);	// weight used in WFQ
    	int weight5 = default(3);	// weight used in WFQ
    	int weight4 = default(3);	// weight used in WFQ
    	int weight3 = default(3);	// weight used in WFQ
    	// N=8 / N=3
    	int weight2 = default(3);	// weight used in WFQ
    	int weight1 = default(3);	// weight used in WFQ
    	int weight0 = default(3);	// weight used in WFQ
    	
    gates:
        input in[];
        output pppg @labels(PPPFrame-conn);
}
