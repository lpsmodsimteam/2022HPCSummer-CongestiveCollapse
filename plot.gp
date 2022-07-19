# To use:
#
# Run simulator once to generate receiver_data.csv and sender_data[i].csv files.
# 
# Make sure that plot.gp is in the same folder as the files mentioned above.
#
# Run gnuplot -c plot.gp [Number of sender nodes in your SST composition].
#
# i.e. I am running 3 sender nodes connected to 1 receiver node.
#      I run: gnuplot -c plot.gp 3

reset session

set datafile separator ','
set terminal png size 1920,1080
set output "plot.png"
set grid
set offset graph 0,0.1,0.1,0
set multiplot layout 2,2 title "Congestive Collapse"

    set title "Queue Size"
    plot "receiver_data.csv" using 1:2 title "Queue Size" with lines 

    set title "Useful Work"
    plot "receiver_data.csv" using 1:3 title "Useful Work" with lines

    set title "Send Rates"
    plot for [i=0:(ARG1-1)] "sender_data".i.".csv" using 1:2 title "Node".i with lines

    set title "New / Total Packets Sent Out"
    plot for [i=0:(ARG1-1)] "sender_data".i.".csv" using 1:3 title "Node".i with lines

unset multiplot