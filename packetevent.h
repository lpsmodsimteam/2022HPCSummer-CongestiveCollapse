#ifndef _packetEvent_H
#define _packetEvent_H

#include <sst/core/event.h>

enum PacketType {
    PACKET,
    ACK,
};

enum StatusType {
    DUP,
    NEW,
};

struct Packet {
    PacketType type;
    StatusType status;
    int id;
};

class PacketEvent : public SST::Event {

public:
    void serialize_order(SST::Core::Serialization::serializer &ser) override {
        Event::serialize_order(ser);
        ser & pack.type;
        ser & pack.status;
        ser & pack.id;
    }

    PacketEvent(Packet pack) :
        Event(),
        pack(pack)
    {}

    PacketEvent() {}

    Packet pack;

    ImplementSerializable(PacketEvent);
};



#endif