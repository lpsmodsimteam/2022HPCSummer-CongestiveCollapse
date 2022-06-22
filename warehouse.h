#ifndef _warehouse_H
#define _warehouse_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include "CommunicationEvent.h"
#include <queue>

class warehouse : public SST::Component {

public:
    warehouse( SST::ComponentId_t id, SST::Params& params );
    ~warehouse();

    void commHandler( SST::Event *ev );

    void setup();

    bool tick( SST::Cycle_t currentCycle );

    SST_ELI_REGISTER_COMPONENT(
        warehouse, // class
        "congestionCollapse", // element library
        "warehouse", // component
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

    int num_of_queues;

    SST::Link *commPort; // Pointer to communication Link
    std::queue<Message> cargoQueue;
    int process_rate;
    int queue_max_size;

    int latest_package; // keeps track of the highest cargo in order to determine if a package is a duplicate or goodput (change this comment)

    std::string clock; // Defining a clock which can be described via unit math as a string.

    float collapse_threshold;

    //warehouse tracks metrics
    float cargo_processed; // Total throughput.
    float cargo_good; // "Goodput"
};

#endif