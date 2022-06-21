#ifndef _communicationEvent_H
#define _communicationEvent_H

#include <sst/core/event.h>

// Message Types
enum DeliveryTypes {
	CARGO,
	ACK,
};

struct Message {
	DeliveryTypes type; // Is this being sent or ack'd.
    int id; // which flow it originates from
    int order_in_crate; // which frame in the window
};

// Custom event type that handles Message structures.
class CommunicationEvent : public SST::Event {

public:
	
	// Serialize members of the Message struct.
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & msg.id;
		ser & msg.order_in_crate;
		ser & msg.type;
	}

	
	CommunicationEvent(Message msg) :
		Event(),
		msg(msg)
	{}

	
	CommunicationEvent() {} // For Serialization only

	Message msg; // Data type handled by event.

	ImplementSerializable(CommunicationEvent); // For serialization.
};

#endif