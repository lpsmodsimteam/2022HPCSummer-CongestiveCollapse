import sst

warehouse = sst.Component("Warehouse", "congestionCollapse.warehouse")
warehouse.addParams({"tickFreq": "1s"})

manufacturer = sst.Component("Manufacturer", "congestionCollapse.manufacturer")
manufacturer.addParams({"tickFreq": "1s"})

sst.Link("Road").connect(
    (warehouse, "commPort", "1ps"), (manufacturer, "commPort", "1ps")
)
