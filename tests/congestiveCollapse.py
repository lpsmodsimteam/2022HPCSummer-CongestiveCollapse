import sst


sender = sst.Component("Sender", "congestiveCollapse.sender")
sender.addParams(
    {
        "tickFreq": "1s",
        "timeout": "9",
        "send_rate": "100",
        "verbose_level": "2",
    }
)

receiver = sst.Component("Receiver", "congestiveCollapse.receiver")
receiver.addParams(
    {
        "tickFreq": "1s",
        "process_rate": "10",
        "verbose_level": "2",
    }
)

sst.Link("Link").connect((sender, "commPort", "1ms"), (receiver, "commPort", "1ms"))
