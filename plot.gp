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
set xlabel "Time (s)"
set offset graph 0,0.1,0.1,0
set multiplot layout 3,2 title "Congestive Collapse"

    set title "Queue Size"
    set key at graph 1, 0.8
    set ylabel "Packets in Queue"
    plot "output/receiver_data.csv" using 1:2 title "Queue Size" with lines lw 3
    
    set key at graph 1, 0.9
    set title "Ratio of New and Retransmitted Packets in Queue"
    set ylabel "Packet Ratio"
    plot "output/receiver_data.csv" using 1:3 title "Packet Ratio" with lines lw 3

    set title "Useful Throughput of Receiver"
    set ylabel "Goodput (%)"
    plot "output/receiver_data.csv" using 1:4 title "Goodput" with lines lw 3

    set title "Send Rates"
    unset ylabel
    plot for [i=0:(ARG1-1)] "output/sender_data".i.".csv" using 1:2 title "Sender-".i with lines lw 3

    set title "New / Total Packets Sent Out"
    plot for [i=0:(ARG1-1)] "output/sender_data".i.".csv" using 1:3 title "Sender-".i with lines lw 3

unset multiplot