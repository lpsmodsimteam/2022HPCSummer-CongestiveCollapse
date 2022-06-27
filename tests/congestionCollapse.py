import sst

warehouse = sst.Component("Warehouse", "congestiveCollapse.warehouse")
warehouse.addParams({"tickFreq": "1s"})

manufacturer = sst.Component("Manufacturer", "congestiveCollapse.manufacturer")
manufacturer.addParams({"tickFreq": "1s"})

sst.Link("Road").connect(
    (warehouse, "commPort", "1ms"), (manufacturer, "commPort", "1ms")
)
