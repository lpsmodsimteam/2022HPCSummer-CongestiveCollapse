#ifndef _manufacturer_H
#define _manufacturer_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <unordered_map>  // hashmap
#include "CommunicationEvent.h"

class manufacturer : public SST::Component {
    
public: 
    manufacturer ( SST::ComponentId_t id, SST::Params& params );
    ~manufacturer();

    bool tick ( SST::Cycle_t currentCycle );

    void commHandler(SST::Event *ev);

    SST_ELI_REGISTER_COMPONENT(
        manufacturer,
        "congestionCollapse",
        "manufacturer",
        SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
        "description",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        {"tickFreq", "Descript", "5s"}
    )

    SST_ELI_DOCUMENT_PORTS(
        {"commPort", "Port where cargo and acknoledgement messages are transported on", {"CommunicationEvent"}}
    )

private:
    SST::Output output;

    SST::Link *commPort;

    std::string clock;

    void sendCargo(int frame, SST::Cycle_t currentCycle, StatusTypes status);

    std::unordered_map<int, int> retransmap;

    bool waiting;
    int node_count;
    int retransmit_time;
    int window_size;
    int prev_window_size;
    int prev_tick_window;
    int64_t message_count;
};

#endif