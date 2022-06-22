#include <sst/core/sst_config.h> 
#include <sst/core/simulation.h>
#include "warehouse.h"
#include <sst/core/stopAction.h>


int tick_test = 0;

warehouse::warehouse( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    output.init(getName() + "->", 1, 0, SST::Output::STDOUT);

    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

    // soon to be Params
    queue_max_size = 5;
    num_of_queues = 1;
    process_rate = 1;
    clock = params.find<std::string>("tickFreq", "1s");

    registerClock(clock, new SST::Clock::Handler<warehouse>(this, &warehouse::tick));

    commPort = configureLink("commPort", new SST::Event::Handler<warehouse>(this, &warehouse::commHandler));
    if ( !commPort ) {
        output.fatal(CALL_INFO, -1, "Failed to configure port 'commPort'\n");
    }
}

warehouse::~warehouse() {

}

void warehouse::setup() {
    std::queue<Message> cargoQueue;
}

bool warehouse::tick( SST::Cycle_t currentCycle) {
    output.output(CALL_INFO, "Time (In Seconds): %ld----------------------\n", getCurrentSimTime());
    if (currentCycle == 10) {
        primaryComponentOKToEndSim();
        return(true);
    }

    // Process queue
    if (!cargoQueue.empty()) {
        for (int i = 0; i < process_rate; i++) {
            // Queue might be come empty during processing, in this case break.
            if (cargoQueue.empty()) {
                break;
            }
            struct Message topCargo = cargoQueue.front(); // Grab cargo at front of queue
            topCargo.type = ACK; // Change type to ACK
            output.output(CALL_INFO, "is processing cargo %d\n", topCargo.order_in_crate);
            commPort->send(new CommunicationEvent(topCargo)); // Send back to manufacturer acknowledging that the message was received.
            cargoQueue.pop(); // Go to next cargo.
        }
    }
        // Send acks
        // Collect data (Eye does it?)
    return(false);
}

void warehouse::commHandler(SST::Event *ev) {
    CommunicationEvent *ce = dynamic_cast<CommunicationEvent*>(ev);
    if ( ce != NULL ) {
        switch(ce->msg.type) 
        {
            case CARGO:
                if (queue_max_size > cargoQueue.size()) {
                    output.output(CALL_INFO, "is queueing cargo %d\n", ce->msg.order_in_crate);
                    cargoQueue.push(ce->msg);
                }
                break;
            case ACK: 
                output.fatal(CALL_INFO, -1, "received an ack. Something is broke!");
                break;
        }
    }
    delete ev; // Clean up event to prevent memory leaks.
}