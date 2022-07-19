import sst


sender = sst.Component("Sender", "congestiveCollapse.sender")
sender.addParams(
    {
        "tickFreq": "1s",
        "timeout": "10",
        "send_rate": "1",
        "verbose_level": "1",
        "node_id": "0",
        "start_cycle": "1",
    }
)

receiver = sst.Component("Receiver", "congestiveCollapse.receiver")
receiver.addParams(
    {
        "tickFreq": "1s",
        "process_rate": "25",
        "verbose_level": "1",
        "num_ports": "1",
    }
)

sst.Link("Link").connect((sender, "commPort", "1ms"), (receiver, "commPort0", "1ms"))
