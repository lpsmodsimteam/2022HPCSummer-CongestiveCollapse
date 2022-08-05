# Congestive Collapse Example

## Prerequisites
```
git clone https://github.com/lpsmodsim/2022HPCSummer-CongestiveCollapse.git
```

# Demonstrate Congestive Collapse
Use the following python driver file.

Copy to 2022HPCSummer-CongestiveCollapse/tests/congestiveCollapse.py
```
# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst  # Use SST library

# REQUIREMENTS:
# If n Sender components are created, the field 'node_id' should be equal to some number 0 to n-1 and should be different for each component.
# Receiver component's field 'num_ports' must be equal to the number of Sender components created.
# i.e. if I create two sender components, one must have node_id '0' and the other node_id '1'. The receiving component will have num_ports '2'.

# Creating a sender component from element congestiveCollapse (congestiveCollapse.sender) named "Sender"
sender = sst.Component("Sender", "congestiveCollapse.sender")

sender_two = sst.Component("Sender2", "congestiveCollapse.sender")

# Adding parameters to sender.
sender.addParams(
    {
        "tickFreq": "1s",  # frequency component updates at.
        "timeout": "5",  # timeout length of packets
        "send_rate": "25",  # number of packets sent per tick.
        "verbose_level": "1",  # level of console verbosity.
        "node_id": "0",  # id of node.
        "start_cycle": "1",  # cycle in which component begins to send packets.
    }
)

sender_two.addParams(
    {
        "tickFreq": "1s",
        "timeout": "5",
        "send_rate": "25",
        "verbose_level": "1",
        "node_id": "1",
        "start_cycle": "1",
    }
)

# Create a receiver and add parameters to it.
receiver = sst.Component("Receiver", "congestiveCollapse.receiver")
receiver.addParams(
    {
        "tickFreq": "1s",  # frequency component updates at.
        "process_rate": "10",  # number of packets consumed per tick.
        "verbose_level": "2",  # level of console verbosity.
        "num_nodes": "2",  # number of ports on the receiver.
        "run_time": "100",  # number of cycles before the simulation ends.
    }
)

# Connect the sender to one of the receiver's ports.
sst.Link("Link_One").connect(
    (sender, "commPort", "1ms"), (receiver, "commPort0", "1ms")
)


sst.Link("Link_Two").connect(
    (sender_two, "commPort", "1ms"), (receiver, "commPort1", "1ms")
)
```

# Running
This is assuming the user is on a system running a Ubuntu-Based Linux Distro.

Prerequisites
```
sudo apt install singularity black mypi
git clone https://github.com/tactcomplabs/sst-containers.git
```
Follow the instructions in the git repo to build the container "sstpackage-11.1.0-ubuntu-20.04.sif".
```
cp sst-containers/singularity/sstpackage-11.1.0-ubuntu-20.04.sif /usr/local/bin/
git clone https://github.com/lpsmodsim/2022HPCSummer-SST.git
sudo ./2022HPCSummer-SST/additions.def.sh
```

Running the model
```
cd 2022HPCSummer-CongestiveCollapse/
make
```

Re-run the model
```
make clean
make
```

Simulation output is generated in 2022HPCSummer-CongestiveCollapse/output

# Plotting

Install gnuplot
```
sudo apt install gnuplot
```

Plot the output data using the provided example script
```
gnuplot example-plot.gp
```

You will see the following output in 2022HPCSummer-CongestiveCollapse/ (click to expand):

plot-packets.png

<img src ="https://raw.githubusercontent.com/lpsmodsim/2022HPCSummer-CongestiveCollapse/main/example_plots/plot_packets.png" width="720">

plot-queue.png

<img src="https://raw.githubusercontent.com/lpsmodsim/2022HPCSummer-CongestiveCollapse/main/example_plots/plot-queue.png" width="720"/>

plot-throughput.png

<img src="https://raw.githubusercontent.com/lpsmodsim/2022HPCSummer-CongestiveCollapse/main/example_plots/plot-throughput.png" width="720"/>

# Generate Documentation

```
sudo apt install doxygen
cd 2022HPCSummer-CongestiveCollapse
doxygen doxygen-conf
```

Doxygen documentation will be generated in 2022HPCSummer-CongestiveCollapse/html 

It can be accessed at 2022HPCSummer-CongestiveCollapse/html/index.html


