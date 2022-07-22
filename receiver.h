#ifndef _receiver_H
#define _receiver_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <queue>
#include "packetevent.h"

/**
 * @brief Receiver Component Class. Receives and processes packets from a sender component. 
 * 
 */
class receiver : public SST::Component {

public:

    /**
     * @brief Construct a new receiver component for the simulation composition.
     * Occurs before the simulation starts.
     * 
     * @param id Component ID tracked by the simulator.
     * @param params Parameters passed in via the Python driver file.
     */
    receiver( SST::ComponentId_t id, SST::Params& params );

    /**
     * @brief Deconstruct the sender component. Occurs after the simulation is finished.
     * 
     */
    ~receiver();
 
    /**
     * @brief Contains the receiver's behavior and runs at its clock frequency.
     * 
     * @param currentCycle Current cycle of the component.
     * @return true Component is finished running.
     * @return false Component is not finished running.
     */
    bool tick ( SST::Cycle_t currentCycle );

    /**
     * @brief Handles packet information received from a sender component.
     * 
     * @param ev PacketEvent that the component received.
     */
    void commHandler(SST::Event *ev);

    /**
     * \cond
     * Currently ignoring SST_ELI Macros as they break doxygen
     */
    SST_ELI_REGISTER_COMPONENT(
        receiver,   // class
        "congestiveCollapse",   // element name
        "receiver",  // component name
        SST_ELI_ELEMENT_VERSION( 1, 0, 0 ), // element version
        "description",  // component description.
        COMPONENT_CATEGORY_UNCATEGORIZED // component category
    )

    // Parameter Name, Description, Default Value.
    SST_ELI_DOCUMENT_PARAMS(
        {"tickFreq", "Descript", "1s"},
        {"process_rate", "Descript", "10"},
        {"verbose_level", "Descript", "1"},
        {"num_nodes", "Number of nodes connected to receiver.", "1"},
    )

    // Port Name, Description, Event Type.
    SST_ELI_DOCUMENT_PORTS(
        {"commPort%d", "Port that packets and acknowledgements are sent across.", {"PacketEvent"}}
    )
    /** 
     * \endcond
     */

private:
    SST::Output output; //!< SST Output object for printing to the console.
    SST::Output csvout; //!< SST Output object for printing to a csv file.
    SST::Link **commPort; //!< Pointer to an array of port pointers. Allows for variable number of ports to be dynamically allocated.

    float packets_received; //!< Total packets in queue.
    float packets_new; //!< New packets in queue.
    float packet_ratio; //!< Ratio of new packets and total packets in queue.

    float new_processed; // goodput
    float total_processed; // throughput
    float work; // ratio of goodput to throughput
    
    float curr_queue_entries_new; //!< New packets entering the queue on latest tick.
    float curr_queue_entries_dup; //!< Retransmissions entering the queue on latest tick.

    std::queue<Packet> infQueue; //!< Infinite queue that stores packets.

    int num_nodes; //!< Number of sender components that are connected to the receiver component.
    int process_rate; //!< Number of packets that are processed per tick.
    std::string clock; //!< Frequency component will tick at. Takes in Unit Algebra string (i.e. "1ms").
    int verbose_level; //!< Verbosity level of console output.
};

#endif