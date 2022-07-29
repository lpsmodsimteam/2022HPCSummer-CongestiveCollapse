import sst
import random

NUM_SENDERS = 2
SEED = 1342

START_CYCLE_MIN = 1
START_CYCLE_MAX = 100

random.seed(SEED)

senders = dict()

for x in range(NUM_SENDERS):
    senders[f"sender_{x}"] = sst.Component(f"Sender_{x}", "congestiveCollapse.sender")
    senders[f"sender_{x}"].addParams(
        {
            "tickFreq": "1s",
            "timeout": "25",
            "send_rate": "10",
            "verbose_level": "2",
            "node_id": f"{x}",
            "start_cycle": f"{random.randint(START_CYCLE_MIN, START_CYCLE_MAX)}",
            # "starting_cycle": "1",
        }
    )

receiver = sst.Component("Receiver", "congestiveCollapse.receiver")
receiver.addParams(
    {
        "tickFreq": "1s",  # frequency component updates at.
        "process_rate": "1000",  # number of packets consumed per tick.
        "verbose_level": "1",  # level of console verbosity.
        "num_nodes": f"{NUM_SENDERS}",  # number of ports on the receiver.
    }
)


# Connect the the sender nodes to the receiver
for x in range(NUM_SENDERS):
    sst.Link(f"Link{x}").connect(
        (receiver, f"commPort{x}", "1ms"), (senders[f"sender_{x}"], "commPort", "1ms")
    )
