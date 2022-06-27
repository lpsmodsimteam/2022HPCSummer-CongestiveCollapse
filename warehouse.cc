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
    queue_max_size = 500;
    num_of_queues = 1;
    process_rate = 5;
    clock = params.find<std::string>("tickFreq", "1s");

    latest_package = 0;
    cargo_good = 0;
    cargo_processed = 0;

    collapse_threshold = 0.2;

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
    std::cout << std::endl;
    output.output(CALL_INFO, "Time (In Seconds): %ld----------------------\n", getCurrentSimTime());
    output.verbose(CALL_INFO, 1, 0, "Total Cargo Processed: %f\n", cargo_processed);
    output.verbose(CALL_INFO, 1, 0, "Useful Cargo Processed: %f\n", cargo_good);
    if (cargoQueue.size() == queue_max_size) {
        output.verbose(CALL_INFO, 1, 0, "Queue is full!\n");
    }
    
    // Determine if the ratio of useful throughput and all throughput reaches a arbitrary threshold to declare congestive collapse.
    if (cargo_processed != 0 && cargo_good / cargo_processed < collapse_threshold) {
        primaryComponentOKToEndSim();
        return(true);
    }

    //if (currentCycle == 9) {
    //   primaryComponentOKToEndSim();
    //    return(true);
    //}

    // Process queue of cargo if its not empty
    if (!cargoQueue.empty()) {
        for (int i = 0; i < process_rate; i++) {
            // Queue might become empty during processing, in this case break.
            if (cargoQueue.empty()) {
                break;
            }
            struct Message topCargo = cargoQueue.front(); // Grab cargo at front of queue
            topCargo.type = ACK; // Change type to ACK
            output.verbose(CALL_INFO, 1, 0, "is processing cargo %d\n", topCargo.order_in_crate);

            // Increment metrics depending on if this cargo is new or is a duplicate.
            if (topCargo.status == NEW) {
               cargo_processed++;
               cargo_good++; 
            } else {
                cargo_processed++;
            }

            commPort->send(new CommunicationEvent(topCargo)); // Send back to manufacturer acknowledging that the message was received.
            cargoQueue.pop(); // Go to next cargo.
        }
    }
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
                } else {
                    output.output(CALL_INFO, "%d has been dropped\n", ce->msg.order_in_crate);
                }
                break;
            case ACK: 
                output.fatal(CALL_INFO, -1, "received an ack. Something is broke!");
                break;
        }
    }
    delete ev; // Clean up event to prevent memory leaks.
}