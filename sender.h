#ifndef _sender_H
#define _sender_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include "packetevent.h"

/**
 * @brief Sender Component Class. Sends packets to a receiver component.
 * 
 */
class sender : public SST::Component {

public:

    /**
     * @brief Construct a new sender component for the simulation composition.
     * Occurs before the simulation starts.
     * 
     * @param id Component ID tracked by the simulator.
     * @param params Parameters passed in via the Python driver file.
     */
    sender ( SST::ComponentId_t id, SST::Params& params );

    /**
     * @brief Deconstruct the sender component. Occurs after the simulation is finished.
     * 
     */
    ~sender();

    /**
     * @brief Contains the sender's behavior and runs at its clock frequency.
     * 
     * @param currentCycle Current cycle of the component.
     * @return true Component is finished running.
     * @return false Component is not finished running.
     */
    bool tick ( SST::Cycle_t currentCycle );

    /**
     * @brief Handles communication infomration received from a receiver component.
     * 
     * @param ev PacketEvent that the component received.
     */
    void commHandler(SST::Event *ev);
 
    /**
     * \cond
     * Currently ignoring SST_ELI Macros as they break doxygen
     */
    SST_ELI_REGISTER_COMPONENT(
        sender,     // Class
        "congestiveCollapse",   // Element library
        "sender",   // Component Name
        SST_ELI_ELEMENT_VERSION( 1, 0, 0 ), // Element version
        "Sends packets via a link to a receiver with an infinite queue.",   // Component Description
        COMPONENT_CATEGORY_UNCATEGORIZED    // Component Category
    )

    // Parameter Name, Description, Default Value
    SST_ELI_DOCUMENT_PARAMS(
        {"tickFreq", "Descript", "1s"},
        {"timeout", "Descript", "100"},
        {"send_rate", "Descript", "10"},
        {"verbose_level", "Descript", "1"},
        {"node_id", "Descript", "1"},
        {"start_cycle", "Descript", "1"},
    )

    // Port Name, Description, Event Type.
    SST_ELI_DOCUMENT_PORTS(
        {"commPort", "Port that packets and acknowledgements are sent across.", {"PacketEvent"}}
    )
    /** 
     * \endcond
     */

private:
    SST::Output output;     //!< SST Output object for printing to the console.
    SST::Output csvout;     //!< SST Output object for printing to a csv file.
    SST::Link *commPort;    //!< Pointer to component's port.

    /**
     * @brief Sends a packet out the sender component's port.
     * 
     * @param id ID of packet.
     * @param currentCycle CHANGE TO SIM TIME?????
     * @param status Status of packet (new or duplicate).
     */
    inline void sendPacket(int id, SST::Cycle_t currentCycle, StatusType status);

    int packets_sent;           //!< Number of packets sent over link.
    int new_packets_sent;       //!< Number of new packets sent over link.
    int retransmissions_sent;   //!< Number of duplicate packets sent over link.
    float packet_ratio;         //!< Ratio of new and total packets being sent over.

    std::map<int, int> time_map; //!< Ordered map which keeps track of packets and when they were sent to determine if timeout has occured for them.

    std::string clock;      //!< Frequency component will tick at. Takes in Unit Algebra string. (i.e. "1ms").
    int time_out;           //!< Number of cycles before retransmission occurs.
    int send_rate;          //!< Number of packets that are sent out per tick.
    int packets_to_send;    //!< Number of packets to send in the current cycle.
    int node_id;            //!< User defined id for sender component.
    int start_cycle;        //!< Cycle where the sender component begins sending packets.
    int verbose_level;      //!< Verbosity level of console output.
};

#endif