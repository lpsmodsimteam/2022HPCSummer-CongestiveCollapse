#ifndef _receiver_H
#define _receiver_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <queue>
#include "packetevent.h"

class receiver : public SST::Component {

public:
    receiver( SST::ComponentId_t id, SST::Params& params );
    ~receiver();

    int verbose_level;
    bool tick ( SST::Cycle_t currentCycle );

    void commHandler(SST::Event *ev);

    SST_ELI_REGISTER_COMPONENT(
        receiver,
        "congestiveCollapse",
        "receiver",
        SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
        "description",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        {"tickFreq", "Descript", "1s"},
        {"process_rate", "Descript", "10"},
        {"verbose_level", "Descript", "1"},
    )

    SST_ELI_DOCUMENT_PORTS(
        {"commPort", "Port that packets and acknowledgements are sent across.", {"PacketEvent"}}
    )

private:
    SST::Output output;
    SST::Link *commPort;

    inline void processPacket();

    float packets_received; // total throughput
    float packets_new; // goodput

    float new_processed;
    float dup_processed;
    
    float curr_queue_entries_new; // new packets entering the queue on latest tick.
    float curr_queue_entries_dup; // retransmissions entering the queue on latest tick.

    std::queue<Packet> infQueue;
    std::queue<Packet> fixedQueue;

    int process_rate;
    std::string clock;
    
};

#endif