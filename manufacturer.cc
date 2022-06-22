#include <sst/core/sst_config.h> 
#include <sst/core/simulation.h>
#include "manufacturer.h"

manufacturer::manufacturer( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    output.init(getName() + "->", 1, 0, SST::Output::STDOUT);

    // soon to be params
    clock = params.find<std::string>("tickFreq", "5s");
    window_size = 100;
    prev_window_size = 0;
    retransmit_time = 20; // retransmit time (in unit time)
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
    int max_cargo_can_send = window_size - prev_window_size; // Does not allow more than window size # of retransmitted cargo to be sent (change this)
    for (auto it : retransmap) {
        if (currentCycle - it.second >= retransmit_time) {
            output.verbose(CALL_INFO, 1, 0, "Resending Cargo %d\n", it.first);
            sendCargo(it.first, currentCycle, RESEND); // Retransmit cargo.
            max_cargo_can_send--;
        }
        if (max_cargo_can_send == 0) {
            break;
        }
    }
    // FIX THIS. More messages should not be sent if max_cargo_can_send is zero. Does a check for that below work out?
    if (!waiting && max_cargo_can_send != 0) {
        for (int i = prev_window_size; i < window_size; i++ ) {
            sendCargo(i, currentCycle, NEW);
            max_cargo_can_send--;
            prev_window_size++;
            if (max_cargo_can_send == 0) {
                break;
            }
        }
    }
    // start transmission timers

    //prev_window_size = window_size;
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
                if (ce->msg.status == NEW) {
                    window_size++;
                }
                waiting = false;
                retransmap.erase(ce->msg.order_in_crate); // Remove cargo from retransmission table.
                break;
        }
    }
}

void manufacturer::sendCargo(int frame, SST::Cycle_t currentCycle, StatusTypes status) {
    DeliveryTypes type = CARGO;
    struct Message newCargo { type, 0, frame, status };
    retransmap[frame] = currentCycle; // Add frame to retransmission table.
    output.output(CALL_INFO, "is sending cargo %d\n", newCargo.order_in_crate);
    commPort->send(new CommunicationEvent(newCargo));
}