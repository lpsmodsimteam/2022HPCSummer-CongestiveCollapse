#include <sst/core/sst_config.h> 
#include <sst/core/simulation.h>
#include "warehouse.h"


int tick_test = 0;

warehouse::warehouse( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    output.init("Warehouse" + getName() + "->", 1, 0, SST::Output::STDOUT);

    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

    // soon to be Params
    queue_max_size = 50;
    num_of_queues = 1;
    process_rate = 50;
    clock = "1s";

    registerClock(clock, new SST::Clock::Handler<warehouse>(this, &warehouse::tick));

    commPort = configureLink("commLink", new SST::Event::Handler<warehouse>(this, &warehouse::commHandler));
    if ( !commPort ) {
        output.fatal(CALL_INFO, -1, "Failed to configure port 'commLink'\n");
    }
}

warehouse::~warehouse() {

}

void warehouse::setup() {
    std::queue<Message> cargoQueue;
}

bool warehouse::tick( SST::Cycle_t currentCycle) {

    if (tick_test == 3) {
        primaryComponentOKToEndSim();
        return(true);
    }

    // Process queue
    if (!cargoQueue.empty()) {
        for (int i = 0; i < process_rate; i++) {
            struct Message topCargo = cargoQueue.front(); // Grab cargo at front of queue
            topCargo.type = ACK; // Change type to ACK
            output.output(CALL_INFO, "Warehouse is processing cargo %d\n", topCargo.order_in_crate);
            commPort->send(new CommunicationEvent(topCargo)); // Send back to manufacturer acknowledging that the message was received.
        }
    }
        // Send acks
        // Collect data (Eye does it?)
    return(false);
    tick_test++;
}

void warehouse::commHandler(SST::Event *ev) {
    CommunicationEvent *ce = dynamic_cast<CommunicationEvent*>(ev);
    if ( ce != NULL ) {
        switch(ce->msg.type) 
        {
            case CARGO:
                if (queue_max_size < cargoQueue.size()) {
                    output.output(CALL_INFO, "Warehouse is queueing cargo %d\n", ce->msg.order_in_crate);
                    cargoQueue.push(ce->msg);
                }
                break;
            case ACK: 
                output.fatal(CALL_INFO, -1, "Warehouse received an ack. Something is broke!");
                break;
        }
    }
    delete ev; // Clean up event to prevent memory leaks.
}