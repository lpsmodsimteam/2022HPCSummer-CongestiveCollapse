import sst

warehouse = sst.Component("Warehouse", "congestionCollapse.warehouse")
warehouse.addParams({"tickFreq": "10s"})

manufacturer = sst.Component("Manufacturer", "congestionCollapse.manufacturer")
manufacturer.addParams({"tickFreq": "10s"})

sst.Link("Road").connect(
    (warehouse, "commPort", "1s"), (manufacturer, "commPort", "1s")
)
