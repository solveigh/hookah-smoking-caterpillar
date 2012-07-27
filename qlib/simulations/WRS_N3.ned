//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package qlib.simulations;

import qlib.Router;
import qlib.Source;
import qlib.Sink;
import qlib.PassiveQueue;

import ned.DatarateChannel;

//
// Single WRS Output port
//
network WRS_N3
{
    @display("bgb=459,231");
    types:
        channel pppline extends DatarateChannel
        {
            delay = 0 s; // propagation delay
            datarate = 1Gbps; //1000000000 bps; // bits per second 1Gbps
            disabled = false;
            ber = 0; // bit-error rate
            per = 0; // packet-error rate
        }
    submodules:
        source: Source {
            @display("p=34,114");
        }
        queue0: PassiveQueue {
            @display("p=186,44");
        }
        queue1: PassiveQueue {
            @display("p=186,114");
        }
        queue2: PassiveQueue {
            @display("p=186,179");
        }

        GbpsSink: Sink {
            @display("p=421,114");
        }
        router: Router {
            @display("p=322,114;b=35,39");
        }
    connections:
        source.pppg++ --> pppline --> queue0.in;
        source.pppg++ --> pppline --> queue1.in;
        source.pppg++ --> pppline --> queue2.in;

        queue0.out --> router.in++;
        queue1.out --> router.in++;
        queue2.out --> router.in++;

        router.pppg --> pppline --> GbpsSink.in;
}
