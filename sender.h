#ifndef _sender_H
#define _sender_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <unordered_map>
#include "packetevent.h"

class sender : public SST::Component {

public:
    sender ( SST::ComponentId_t id, SST::Params& params );
    ~sender();

    bool tick ( SST::Cycle_t currentCycle );

    void commHandler(SST::Event *ev);
    int verbose_level;
    SST_ELI_REGISTER_COMPONENT(
        sender,
        "congestiveCollapse",
        "sender",
        SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
        "Sends packets via a link to a receiver with an infinite queue.",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        {"tickFreq", "Descript", "1s"},
        {"timeout", "Descript", "100"},
        {"send_rate", "Descript", "10"},
        {"verbose_level", "Descript", "1"},
    )

    SST_ELI_DOCUMENT_PORTS(
        {"commPort", "Port that packets and acknowledgements are sent across.", {"PacketEvent"}}
    )

private:
    SST::Output output;
    SST::Link *commPort;

    inline void sendPacket(int id, SST::Cycle_t currentCycle, StatusType status);

    int packets_sent;   
    int retransmissions_sent;

    std::unordered_map<int, int> time_map;

    std::string clock;
    int time_out;
    int send_rate;
    int packets_to_send;
    
};

#endif