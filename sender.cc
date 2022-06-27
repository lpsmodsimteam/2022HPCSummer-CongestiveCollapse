#include <sst/core/sst_config.h>
#include <sst/core/simulation.h>
#include "sender.h"


sender::sender( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    
    // Initialize parameters
    clock = params.find<std::string>("tickFreq", "1s");
    time_out = params.find<int64_t>("timeout", 10);
    send_rate = params.find<int64_t>("send_rate", 20);
    verbose_level = params.find<int64_t>("verbose_level", 1);
    
    output.init(getName() + "->", verbose_level, 0, SST::Output::STDOUT);
    // Initialize stats
    packets_sent = 0;   
    new_packets_sent = 0;
    retransmissions_sent = 0;

    // Initializing Vector 

    registerClock(clock, new SST::Clock::Handler<sender>(this, &sender::tick));
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
    //std::cout << send_rate << std::endl;
    packets_to_send = send_rate;
    
    // Check Transmission Table First
    // Send any packets that need to be retransmitted.

    /**
    for (auto it = time_map.begin(); it != time_map.end(); it++) {
        if (currentCycle - it->second >= time_out) {
            output.verbose(CALL_INFO, 3, 0, "Sending duplicate Packet %d\n", it->first);

            // Changed from packets_sent to it->first
            sendPacket(it->first, currentCycle, DUP);
            retransmissions_sent++;
            packets_sent++;
            packets_to_send--;
        }
        if (packets_to_send == 0) {
            break;
        }
    }
    */

    /** Attempt at vector for keeping track of times. 
    for (int i = 0; i < time_vec.size(); i++) {
        output.verbose(CALL_INFO, 5, 0, "Checking Vector index: %d\nVector size: %d", i, time_vec.size());
        if ((time_vec[i] != 0) && (currentCycle - time_vec[i] >= time_out)) {
            sendPacket(i, currentCycle, DUP);
            retransmissions_sent++;
            packets_sent++;
            packets_to_send--;
        }
        if (packets_to_send == 0) {
            break;
        }
    } */

    for (auto it : time_map) {
        if (currentCycle - it.second > time_out) {
            output.verbose(CALL_INFO, 3, 0, "Sending duplicate Packet %d\n", it.first);
            output.verbose(CALL_INFO, 4, 0, "Duplicate Packet %d: | Current Cycle: %ld | Sent Cycle: %d\n", it.first, currentCycle, it.second);
            // Changed from packets_sent to it.first
            sendPacket(it.first, currentCycle, DUP);
            retransmissions_sent++;
            packets_sent++;
            packets_to_send--;
        }
        if (packets_to_send == 0) {
            break;
        }
    } 

    // Send new packets.
    for (int i = 0; i < packets_to_send; i++) {
        output.verbose(CALL_INFO, 3, 0, "Sending new Packet %d\n", packets_sent);
        sendPacket(packets_sent, currentCycle, NEW);
        packets_sent++;
        new_packets_sent++;
    }

    // Modify Stats
    return(false);
}   

void sender::commHandler(SST::Event *ev) {
    PacketEvent *pe = dynamic_cast<PacketEvent*>(ev);
    if (pe != NULL ) {
        switch (pe->pack.type) {
            case PACKET:
                output.fatal(CALL_INFO, -1, "Sender should not receive packets. Something is wrong!");
                break;
            case ACK:
                output.verbose(CALL_INFO, 5, 0, "Received ACK for Packet %d\n", pe->pack.id);
                // Packet info here? Verbose 6?
                
                
                // Received ACK, delete ID from transmission table.
                time_map.erase(pe->pack.id); 
        }
    }
    delete ev;
}

void sender::sendPacket(int id, SST::Cycle_t currentCycle, StatusType status) {
    PacketType type = PACKET;
    Packet packet = { type, status, id };

    time_map[id] = currentCycle + 1; 

    commPort->send(new PacketEvent(packet));
}