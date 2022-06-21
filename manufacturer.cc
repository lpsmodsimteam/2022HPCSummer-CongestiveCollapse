#include <sst/core/sst_config.h> 
#include <sst/core/simulation.h>
#include "manufacturer.h"

manufacturer::manufacturer( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    output.init("manufacturer-" + getName() + "->", 1, 0, SST::Output::STDOUT);

    // soon to be params
    clock = "1s";
    window_size = 10;
    retransmit_time = 10; // retransmit time (in seconds)

    message_count = 0;

    registerClock(clock, new SST::Clock::Handler<manufacturer>(this, &manufacturer::tick));
    commPort = configureLink("commLink", new SST::Event::Handler<manufacturer>(this, &manufacturer::commHandler));

    if ( !commPort ) {
        output.fatal(CALL_INFO, -1, "Failed to configure port 'commLink'\n");
    }
}

manufacturer::~manufacturer() {

}

bool manufacturer::tick( SST::Cycle_t currentCycle ) {
    for (int i = 0; i < window_size; i++ ) {
        sendCargo(i);
    }
    // start transmission timers
    return(false);
}

void manufacturer::commHandler(SST::Event *ev) {
    CommunicationEvent *ce = dynamic_cast<CommunicationEvent*>(ev);
    if (ce != NULL) {
        switch(ce->msg.type) 
        {
            case CARGO:
                output.fatal(CALL_INFO, -1, "Manufacturer received cargo. Something is broke!");
                break;
            case ACK: 
                output.output(CALL_INFO, "Cargo %d has been received\n", ce->msg.order_in_crate);
                break;
        }
    }
}

void manufacturer::sendCargo(int frame) {
    struct Message newCargo { CARGO, 0, frame };
    output.output(CALL_INFO, "Manufacturer is sending cargo %d\n", newCargo.order_in_crate);
    commPort->send(new CommunicationEvent(newCargo));
}