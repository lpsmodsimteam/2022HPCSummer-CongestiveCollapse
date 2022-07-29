#include <sst/core/sst_config.h>
#include <sst/core/simulation.h>
#include "receiver.h"

// change to param
//const int queue_input_sample = 100;
//int fixed_queue[queue_input_sample];
//int fixed_queue_count;

receiver::receiver( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {

    // Initialize Parameters
    clock = params.find<std::string>("tickFreq", "1s");
    process_rate = params.find<int64_t>("process_rate", 10);
    verbose_level = params.find<int64_t>("verbose_level", 1);
    num_nodes = params.find<int64_t>("num_nodes", 1);

    // Enabling SST Console Output.
    output.init(getName() + "->", verbose_level, 0, SST::Output::STDOUT);

    // Enabling SST File Output
    csvout.init("CSVOUT", 1, 0, SST::Output::FILE, "output/receiver_data.csv");
    csvout.output("Time,Queue Size,Useful Work,New/Total Packets Entering Queue\n");

    // Stats
    packets_received = 0;
    packets_new = 0; 
    curr_queue_entries_new = 0;
    curr_queue_entries_dup = 0;

    // (?)
    //fixed_queue_count = 0;

    // Register the node as a primary component.
	// Then declare that the simulation cannot end until this
	// primary component declares primaryComponentOKToEndSim();
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

    // Register Clock
    registerClock(clock, new SST::Clock::Handler<receiver>(this, &receiver::tick));
 
    // Pointer to an array of port pointers.
    commPort = new SST::Link*[num_nodes];

    // Configure all ports to a different link
    for (int i = 0; i < num_nodes; ++i) {
        std::string strport = "commPort" + std::to_string(i);
        commPort[i] = configureLink(strport, new SST::Event::Handler<receiver>(this, &receiver::commHandler));
        if (!commPort) {
            output.fatal(CALL_INFO, -1, "Failled to configure port 'commPort'\n");
        }
    }
   
}

receiver::~receiver() {

}

bool receiver::tick( SST::Cycle_t currentCycle ) {
    output.verbose(CALL_INFO, 2, 0, "SimTime (In Seconds): ------------ %ld\n", getCurrentSimTime());
    output.verbose(CALL_INFO, 2, 0, "Throughput: %f\n", packets_received);
    output.verbose(CALL_INFO, 2, 0, "Goodput: %f\n", packets_new); 
    output.verbose(CALL_INFO, 2, 0, "Queue Size: %ld\n", infQueue.size()); 

    // End after cycle (For collecting statistics)
    if (currentCycle == 300) {
        primaryComponentOKToEndSim();
        return(true);
    }
 
    if (!infQueue.empty()) {
        // Process messages in queue.
        for (int i = 0; i < process_rate; i++) {
            if (infQueue.empty()) {
                break; // If the queue becomes empty, break out.
            }
            Packet packet = infQueue.front(); // Grab packet at front of queue
 
            output.verbose(CALL_INFO, 4, 0, "Consuming packet %d for node %d\n", packet.id, packet.node_id);

            // Update count of packets and type in queue.
            if (packet.status == NEW) {
                packets_new--;
                packets_received--;

                new_processed++;
                total_processed++;    
            } else {
                total_processed++;
                packets_received--;
            }

            /**
            if (packet.status == NEW) {
                new_processed++; 
                total_processed++;
            } else {
                total_processed++;   
            }*/

            // Send an ack.
            packet.type = ACK;
            commPort[packet.node_id]->send(new PacketEvent(packet));

            infQueue.pop(); // "Process" the packet removing it from the queue.
        }
    }
 
    if (packets_received != 0) {
        packet_ratio = packets_new / packets_received; 
    } else {
        packet_ratio = 0;
    }

    if (total_processed != 0) {
        work = new_processed / total_processed;
    } else {
        work = 0;
    }

    new_processed = 0;
    total_processed = 0;

    csvout.output("%ld,%ld,%f,%f\n", getCurrentSimTime(), infQueue.size(), packet_ratio, work);
     
    return(false); 
}

void receiver::commHandler(SST::Event *ev) {
    PacketEvent *pe = dynamic_cast<PacketEvent*>(ev); // Cast the incoming event to a PacketEvent pointer.
    if (pe != NULL) {
        switch (pe->pack.type) {
            case PACKET:
                // Add messages to infinite queue.
                infQueue.push(pe->pack);    

                // Count and type of packet entering queue.
                if (pe->pack.status == NEW) {
                    packets_new++;
                    packets_received++;
                } else {
                    packets_received++;
                }  
                break;
            case ACK:
                output.fatal(CALL_INFO, -1, "The receiver should not be receiving acks. Something is wrong!");
                break;
        }
    }
    delete ev; // Delete event to prevent memory leaks.
}