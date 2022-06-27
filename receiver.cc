#include <sst/core/sst_config.h>
#include <sst/core/simulation.h>
#include "receiver.h"

// change to param
int queue_input_sample = 100;

receiver::receiver( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    // Initialize parameters
    clock = params.find<std::string>("tickFreq", "1s");
    process_rate = params.find<int64_t>("process_rate", 10);
    verbose_level = params.find<int64_t>("verbose_level", 1);

    output.init(getName() + "->", verbose_level, 0, SST::Output::STDOUT);

    // Initialize stats
    packets_received = 0;
    packets_new = 0; 
    curr_queue_entries_new = 0;
    curr_queue_entries_dup = 0;

    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

    registerClock(clock, new SST::Clock::Handler<receiver>(this, &receiver::tick));
    commPort = configureLink("commPort", new SST::Event::Handler<receiver>(this, &receiver::commHandler));
    if (!commPort) {
        output.fatal(CALL_INFO, -1, "Failled to configure port 'commPort'\n");
    }
}

receiver::~receiver() {

}

bool receiver::tick( SST::Cycle_t currentCycle ) {

    output.verbose(CALL_INFO, 1, 0, "SimTime (In Seconds): ------------ %ld\n", getCurrentSimTime());
    output.verbose(CALL_INFO, 2, 0, "Throughput: %f\n", packets_received);
    output.verbose(CALL_INFO, 2, 0, "Goodput: %f\n", packets_new); 
    output.verbose(CALL_INFO, 2, 0, "Queue Size: %ld\n", infQueue.size());
 

    //std::cout << infQueue.size() << std::endl;

    //std::cout << getCurrentSimTime() << std::endl;
    //if (packets_new != 0 && packets_received != 0) {
    //    std::cout << packets_new / (float) packets_received << std::endl;
    //}

    // End when ratio of goodput to total throughput goes below a threshold.
    /**if (packets_new != 0 && packets_received !=0 && (packets_new / (float) packets_received <= 0.2)) {
        primaryComponentOKToEndSim();
        return(true);
    } */

    // End after cycle (For collecting statistics)
    if (currentCycle == 1000) {
        primaryComponentOKToEndSim();
        return(true);
    }

    // Intermediate fixed size queue on the link for collecting statistics.


    if (!infQueue.empty()) {
        // Process messages in queue.
        for (int i = 0; i < process_rate; i++) {
            if (infQueue.empty()) {
                break; // If the queue becomes empty, break out.
            }
            Packet packet = infQueue.front(); // Grab packet at front of queue
 
            //Update metric regarding ratio of goodput to throughput.
            if (packet.status == NEW) {
                packets_new--;
                packets_received--;
            } else {
                packets_received--;
            }

            /**if (packet.status == NEW) {
                new_processed++;
                std::cout << "New processed" << std::endl;
            } else {
                dup_processed++;
                std::cout << "Dup Processed" << std::endl;
            } */


            // Send an ack.
            packet.type = ACK;
            commPort->send(new PacketEvent(packet));

            infQueue.pop(); // "Process" the packet removing it from the queue.
        }
    }

    /**if (new_processed == 0 && currentCycle != 1) {
        primaryComponentOKToEndSim();
        return(true);
    }

    new_processed = 0;
    dup_processed = 0; */

    //curr_queue_entries_new = 0;
    //test = 0;

    return(false);

    // Update statistics
}

void receiver::commHandler(SST::Event *ev) {
    PacketEvent *pe = dynamic_cast<PacketEvent*>(ev);
    if (pe != NULL) {
        switch (pe->pack.type) {
            case PACKET:
                // Add messages to infinite queue.
                infQueue.push(pe->pack);
                
                // Measure current type of packets entering the input queue per second for the link.
                if (pe->pack.status == NEW) { 
                    curr_queue_entries_new++;
                } else {
                    curr_queue_entries_dup++;
                }

                // Collect metric regarding ratio of goodput to throughput.
                if (pe->pack.status == NEW) {
                    packets_new++;
                    packets_received++;
                } else {
                    packets_received++;
                }

                // Measure junk in queue? (Increment here and decrement during process?)
                // Mesaure valid in queue? (Increment here and decrement during process?)

                // add statistics printout condition
                //std::cout << curr_queue_entries_new << std::endl;

                /**if (curr_queue_entries_new == 0) {
                    primaryComponentOKToEndSim();
                } */

                if (getCurrentSimTime() % 2 == 0) {   
                    //std::cout << curr_queue_entries_new << std::endl;
                    curr_queue_entries_dup = 0;
                    curr_queue_entries_new = 0;
                }
                

                break;
            case ACK:
                output.fatal(CALL_INFO, -1, "The receiver should not be receiving acks. Something is wrong!");
                break;
        }
    }
    delete ev;
}