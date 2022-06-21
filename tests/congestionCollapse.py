import sst

warehouse = sst.Component("Warehouse", "congestionCollapse.warehouse")

manufacturer = sst.Component("Manufacturer", "congestionCollapse.manufacturer")

sst.Link("Road").connect(
    (warehouse, "commPort", "1ps"), (manufacturer, "commPort", "1ps")
)
