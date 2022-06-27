import sst


sender = sst.Component("Sender", "congestiveCollapse.sender")
sender.addParams(
    {
        "tickFreq": "1s",
        "timeout": "10",
        "send_rate": "10",
        "verbose_level": "5",
    }
)

receiver = sst.Component("Receiver", "congestiveCollapse.receiver")
receiver.addParams(
    {
        "tickFreq": "1s",
        "process_rate": "10",
        "verbose_level": "5",
    }
)

sst.Link("Link").connect((sender, "commPort", "1ps"), (receiver, "commPort", "1ps"))
