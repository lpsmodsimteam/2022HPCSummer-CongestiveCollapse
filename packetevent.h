#ifndef _packetEvent_H
#define _packetEvent_H
#include <sst/core/event.h>

/**
 * @brief Type of packet.
 * 
 */
enum PacketType {
    PACKET, /**< Type for packets */
    ACK,    /**< Type for acknowledgements for packets. */
};

/**
 * @brief Status for packet.  
 * 
 */
enum StatusType {
    DUP,    /**< Duplicate status */
    NEW,    /**< New status */
};

/**
 * @brief Packet Structure. Contains information regarding the packet's type and status. Its id and what node sent it and when. 
 * 
 */
struct Packet {
    PacketType type;
    StatusType status;
    int id;
    SST::Cycle_t time_sent;
    int node_id;
};

/**
 * @brief Custom event type that handles Packet structures.
 * 
 */
class PacketEvent : public SST::Event {

public:
    /**
     * @brief Serialize members of the Packet structure.
     * 
     * @param ser Wrapper class for objects to declare the order in which their members are serialized/deserialized.
     */
    void serialize_order(SST::Core::Serialization::serializer &ser) override {
        Event::serialize_order(ser);
        ser & pack.type;
        ser & pack.status;
        ser & pack.id;
        ser & pack.time_sent;
        ser & pack.node_id;
    }

    PacketEvent(Packet pack) :
        Event(),
        pack(pack)
    {}

    PacketEvent() {} // For serialization.

    Packet pack; // Data type handled by event.

    ImplementSerializable(PacketEvent); // For serialization.
};

#endif