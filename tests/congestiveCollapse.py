# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst  # Use SST library

# REQUIREMENTS:
# If n Sender components are created, the field 'node_id' should be equal to some number 0 to n-1 and should be different for each component.
# Receiver component's field 'num_ports' must be equal to the number of Sender components created.
# i.e. if I create two sender components, one must have node_id '0' and the other node_id '1'. The receiving component will have num_ports '2'.

# Creating a sender component from element congestiveCollapse (congestiveCollapse.sender) named "Sender"
sender = sst.Component("Sender", "congestiveCollapse.sender")

sender_two = sst.Component("Sender2", "congestiveCollapse.sender")

# Adding parameters to sender.
sender.addParams(
    {
        "tickFreq": "1ms",  # frequency component updates at.
        "timeout": "10",  # timeout length of packets
        "send_rate": "1",  # number of packets sent per tick.
        "verbose_level": "1",  # level of console verbosity.
        "node_id": "0",  # id of node.
        "start_cycle": "1",  # cycle in which component begins to send packets.
    }
)


sender_two.addParams(
    {
        "tickFreq": "1ms",
        "timeout": "10",
        "send_rate": "1",
        "verbose_level": "1",
        "node_id": "1",
        "start_cycle": "1",
    }
)

# Create a receiver and add parameters to it.
receiver = sst.Component("Receiver", "congestiveCollapse.receiver")
receiver.addParams(
    {
        "tickFreq": "1ms",  # frequency component updates at.
        "process_rate": "1",  # number of packets consumed per tick.
        "verbose_level": "2",  # level of console verbosity.
        "num_nodes": "2",  # number of ports on the receiver.
    }
)

# Connect the sender to one of the receiver's ports.
sst.Link("Link_One").connect((sender, "commPort", "1ms"), (receiver, "commPort0", "1ms"))


sst.Link("Link_Two").connect(
    (sender_two, "commPort", "1ms"), (receiver, "commPort1", "1ms")
)
