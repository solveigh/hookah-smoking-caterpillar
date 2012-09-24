
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
        string routingAlgorithm @enum("PRIO", "RR", "DRR", "LQF+", "WRQ", "FQSW") = default("PRIO");
        volatile int randomGateIndex = default(intuniform(0, sizeof(out)-1));    // the destination gate in case of random routing
        
        int bandwidth = default(125000000);		// bandwidth in bytes (1Gbps = 125,000,000 bytes per second)
        
        double startTime @unit(s) = default(0); // when the module sends out the first packet
        volatile double serviceTime @unit(s); // WRS cycle
        volatile double ifg @unit(s); 				// time for interframe gap
    	int nofCoS = default(8);	// default: 7..0

    	// N=8
    	int wfq_weight7 = default(200);	// weight used by WFQ
    	int wfq_weight6 = default(2);	// weight used by WFQ
    	int wfq_weight5 = default(2);	// weight used by WFQ
    	int wfq_weight4 = default(2);	// weight used by WFQ
    	int wfq_weight3 = default(2);	// weight used by WFQ
    	// N=8 / N=3
    	int wfq_weight2 = default(2);	// weight used by WFQ
    	int wfq_weight1 = default(2);	// weight used by WFQ
    	int wfq_weight0 = default(1);	// weight used by WFQ
    	
    	// N=8
    	int wrq_weight7 = default(200);	// weight used by WRQ
    	int wrq_weight6 = default(2);	// weight used by WRQ
    	int wrq_weight5 = default(2);	// weight used by WRQ
    	int wrq_weight4 = default(2);	// weight used by WRQ
    	int wrq_weight3 = default(2);	// weight used by WRQ
    	// N=8 / N=3
    	int wrq_weight2 = default(2);	// weight used by WRQ
    	int wrq_weight1 = default(2);	// weight used by WRQ
    	int wrq_weight0 = default(1);	// weight used by WRQ

    	// N=8
    	int drr_weight7 = default(10);	// weight used by WRR
    	int drr_weight6 = default(6);	// weight used by WRR
    	int drr_weight5 = default(5);	// weight used by WRR
    	int drr_weight4 = default(4);	// weight used by WRR
    	int drr_weight3 = default(3);	// weight used by WRR
    	// N=8 / N=3
    	int drr_weight2 = default(3);	// weight used by WRR
    	int drr_weight1 = default(2);	// weight used by WRR
    	int drr_weight0 = default(1);	// weight used by WRR
    	
    gates:
        input in[];
        output pppg @labels(PPPFrame-conn);
}
