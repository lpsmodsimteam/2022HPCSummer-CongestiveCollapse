#include <sst/core/sst_config.h>
#include <sst/core/simulation.h>
#include "sender.h"

sender::sender( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    
    // Initialize Parameters
    clock = params.find<std::string>("tickFreq", "1s");
    time_out = params.find<int64_t>("timeout", 10);
    send_rate = params.find<int64_t>("send_rate", 20);
    verbose_level = params.find<int64_t>("verbose_level", 1);
    node_id = params.find<int64_t>("node_id", 1);
    start_cycle = params.find<int64_t>("start_cycle", 1);
    
    // Enabling SST Console Output
    output.init(getName() + "->", verbose_level, 0, SST::Output::STDOUT);
    
    // Create a string for each sender's filename to output data to.
    std::string fileName = "sender_data" + std::to_string(node_id) + ".csv";
    csvout.init("CSVOUT", 1, 0, SST::Output::FILE, fileName);
    csvout.output("Time,Send Rate,New/Total Packets Entering Queue\n");

    // Stats
    packets_sent = 0;   
    new_packets_sent = 0;
    retransmissions_sent = 0; 

    // Register Clock
    registerClock(clock, new SST::Clock::Handler<sender>(this, &sender::tick));

    // Configure Port
    commPort = configureLink("commPort", new SST::Event::Handler<sender>(this, &sender::commHandler));
    if (!commPort) {
        output.fatal(CALL_INFO, -1, "Failled to configure port 'commPort'\n");
    }
}

sender::~sender() {

}

bool sender::tick( SST::Cycle_t currentCycle ) {
    output.verbose(CALL_INFO, 2, 0, "SimTime (In Seconds): ------------ %ld\n", getCurrentSimTime()); 
    
    //send_rate = send_rate + 1; 
    if (start_cycle <= currentCycle) {
        //send_rate = send_rate + 1;
        packets_to_send = send_rate;
    
        // Check Transmission Table First
        // Send any packets that need to be retransmitted.
        for (auto it : time_map) {
            if (currentCycle - it.second >= time_out && it.second != 0) {
                output.verbose(CALL_INFO, 3, 0, "Sending duplicate Packet %d\n", it.first);
                output.verbose(CALL_INFO, 4, 0, "Duplicate Packet %d: | Current Cycle: %ld | Sent Cycle: %d\n", it.first, currentCycle, it.second);
                // Changed from packets_sent to it.first
                sendPacket(it.first + packets_sent, currentCycle, DUP);
                retransmissions_sent++;
                packets_sent++;
                packets_to_send--;
            }
            if (packets_to_send == 0) {
                break;
            }
        }

        // Then send new packets.
        for (int i = 0; i < packets_to_send; i++) {
            output.verbose(CALL_INFO, 3, 0, "Sending new Packet %d\n", packets_sent);
            sendPacket(packets_sent, currentCycle, NEW);
            packets_sent++;
            new_packets_sent++;
        }
    }

    packet_ratio = new_packets_sent / (float) send_rate; 
    csvout.output("%ld,%d,%f\n", getCurrentSimTime(), send_rate, packet_ratio);

    new_packets_sent = 0;

    // Modify Stats
    return(false);
}   

void sender::commHandler(SST::Event *ev) {
    PacketEvent *pe = dynamic_cast<PacketEvent*>(ev); // Cast the incoming event to a PacketEvent pointer.
    if (pe != NULL ) {
        switch (pe->pack.type) {
            case PACKET:
                output.fatal(CALL_INFO, -1, "Sender should not receive packets. Something is wrong!");
                break;
            case ACK:
                output.verbose(CALL_INFO, 5, 0, "Received ACK for Packet %d\n", pe->pack.id); 

                // Received ACK, delete ID from transmission table.
                time_map.erase(pe->pack.id);
                //time_map[pe->pack.id] = 0; 
        }
    }
    delete ev;
}

// Construct and send packets out the sender's port.
void sender::sendPacket(int id, SST::Cycle_t currentCycle, StatusType status) {
    PacketType type = PACKET;
    Packet packet = { type, status, id, currentCycle, node_id };

    time_map[id] = currentCycle + 1;

    commPort->send(new PacketEvent(packet));
}