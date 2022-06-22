#include <sst/core/sst_config.h> 
#include <sst/core/simulation.h>
#include "manufacturer.h"

manufacturer::manufacturer( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    output.init(getName() + "->", 1, 0, SST::Output::STDOUT);

    // soon to be params
    clock = params.find<std::string>("tickFreq", "1s");
    window_size = 10;
    prev_window_size = 0;
    retransmit_time = 2; // retransmit time (in unit time)
    waiting = false; // wait for ack before increasing window size and sending more messages.

    message_count = 0;

    registerClock(clock, new SST::Clock::Handler<manufacturer>(this, &manufacturer::tick));
    commPort = configureLink("commPort", new SST::Event::Handler<manufacturer>(this, &manufacturer::commHandler));

    if ( !commPort ) {
        output.fatal(CALL_INFO, -1, "Failed to configure port 'commPort'\n");
    }
}

manufacturer::~manufacturer() {

}

bool manufacturer::tick( SST::Cycle_t currentCycle ) {
    output.output(CALL_INFO, "Time (In Seconds): %ld-------------------\n", getCurrentSimTime());

    // Scan retransmission map for any cargo that needs to be resent.
    for (auto it : retransmap) {
        if (currentCycle - it.second >= retransmit_time) {
            std::cout << it.first << " needs to be retransmitted" << std::endl;
        }
    }

    if (!waiting) {
        for (int i = prev_window_size; i < window_size; i++ ) {
            sendCargo(i, currentCycle);
        }
    }
    // start transmission timers

    prev_window_size = window_size;
    waiting = true;
    return(false);
}

void manufacturer::commHandler(SST::Event *ev) {
    CommunicationEvent *ce = dynamic_cast<CommunicationEvent*>(ev);
    if (ce != NULL) {
        switch(ce->msg.type) 
        {
            case CARGO:
                output.fatal(CALL_INFO, -1, "received cargo. Something is broke!");
                break;
            case ACK: 
                output.output(CALL_INFO, "Cargo %d has been received\n", ce->msg.order_in_crate);
                window_size++;
                waiting = false;
                retransmap.erase(ce->msg.order_in_crate); // Remove cargo from retransmission table.
                break;
        }
    }
}

void manufacturer::sendCargo(int frame, SST::Cycle_t currentCycle) {
    DeliveryTypes type = CARGO;
    struct Message newCargo { type, 0, frame };
    retransmap[frame] = currentCycle; // Add frame to retransmission table.
    output.output(CALL_INFO, "is sending cargo %d\n", newCargo.order_in_crate);
    commPort->send(new CommunicationEvent(newCargo));
}