
package qlib;

//
// Sends the messages to different outputs depending on a set algorithm.
//
// @author rhornig
// @todo minDelay not implemented
//
simple Scheduler
{
    parameters:
        @group(Queueing);
        @display("i=block/routing");
        string routingAlgorithm @enum("FCFS", "RR", "WRR", "SQF", "LQF", "WFQ", "FQSW") = default("FCFS");
        volatile int randomGateIndex = default(intuniform(0, sizeof(out)-1));    // the destination gate in case of random routing
        
        double startTime @unit(s) = default(0); // when the module sends out the first packet
        volatile double serviceTime @unit(s); // WRS cycle
        volatile double ifg @unit(s); 				// time for interframe gap
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

    	// N=8
    	int wrr_weight7 = default(10);	// weight used in WRR
    	int wrr_weight6 = default(6);	// weight used in WRR
    	int wrr_weight5 = default(5);	// weight used in WRR
    	int wrr_weight4 = default(4);	// weight used in WRR
    	int wrr_weight3 = default(3);	// weight used in WRR
    	// N=8 / N=3
    	int wrr_weight2 = default(3);	// weight used in WRR
    	int wrr_weight1 = default(2);	// weight used in WRR
    	int wrr_weight0 = default(1);	// weight used in WRR
    	
    gates:
        input in[];
        output pppg @labels(PPPFrame-conn);
}
