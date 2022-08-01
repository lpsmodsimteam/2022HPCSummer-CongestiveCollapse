# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst # Use SST Library
import random

NUM_SENDERS = 2 # Number of sender nodes to instantiate.
SEED = 1342 # Seed for RNG

# Lower and upper range for the sender component's starting cycle
START_CYCLE_MIN = 1
START_CYCLE_MAX = 100

random.seed(SEED)

senders = dict()

for x in range(NUM_SENDERS):

    # Creating a sender component from element tcpGlobSync (tcpGlobSync.sender) named "Sender_{node_id}". 
    senders[f"sender_{x}"] = sst.Component(f"Sender_{x}", "congestiveCollapse.sender")
    
    # Adding parameters to sender node.
    senders[f"sender_{x}"].addParams(
        {
            "tickFreq": "1s", # frequency component updates at.
            "timeout": "25", # timeout length of packets.
            "send_rate": "10", # number of packets sent per tick.
            "verbose_level": "2", # level of console verbosity.
            "node_id": f"{x}", # id of node.
            "start_cycle": f"{random.randint(START_CYCLE_MIN, START_CYCLE_MAX)}", # cycle in which component begins to tick. 
        }
    )

# Create a receiver and add parameters to it.
receiver = sst.Component("Receiver", "congestiveCollapse.receiver")
receiver.addParams(
    {
        "tickFreq": "1s",  # frequency component updates at.
        "process_rate": "1000",  # number of packets consumed per tick.
        "verbose_level": "1",  # level of console verbosity.
        "num_nodes": f"{NUM_SENDERS}",  # number of ports on the receiver.
    }
)


# Connect each sender node to the receiver
for x in range(NUM_SENDERS):
    sst.Link(f"Link{x}").connect(
        (receiver, f"commPort{x}", "1ms"), (senders[f"sender_{x}"], "commPort", "1ms")
    )
