# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst  # Use SST library

# Creating a sender component from element congestiveCollapse (congestiveCollapse.sender) named "Sender"
sender = sst.Component("Sender", "congestiveCollapse.sender")

sender_two = sst.Component("Sender2", "congestiveCollapse.sender")

# Adding parameters to sender.
sender.addParams(
    {
        "tickFreq": "1s",  # frequency component updates at.
        "timeout": "10",  # timeout length of packets
        "send_rate": "30",  # number of packets sent per tick.
        "verbose_level": "4",  # level of console verbosity.
        "node_id": "0",  # id of node.
        "start_cycle": "1",  # cycle in which component begins to send packets.
    }
)


# Adding parameters to sender.
sender_two.addParams(
    {
        "tickFreq": "1s",  # frequency component updates at.
        "timeout": "10",  # timeout length of packets
        "send_rate": "30",  # number of packets sent per tick.
        "verbose_level": "4",  # level of console verbosity.
        "node_id": "1",  # id of node.
        "start_cycle": "1",  # cycle in which component begins to send packets.
    }
)

receiver = sst.Component("Receiver", "congestiveCollapse.receiver")
receiver.addParams(
    {
        "tickFreq": "1s",  # frequency component updates at.
        "process_rate": "10",  # number of packets consumed per tick.
        "verbose_level": "4",  # level of console verbosity.
        "num_ports": "2",  # number of ports on the receiver.
    }
)

# Connect the sender to one of the receiver's ports.
sst.Link("Link").connect((sender, "commPort", "1ms"), (receiver, "commPort0", "1ms"))

sst.Link("Linktwo").connect(
    (sender_two, "commPort", "1ms"), (receiver, "commPort1", "1ms")
)
