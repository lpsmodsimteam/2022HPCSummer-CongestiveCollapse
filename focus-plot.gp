reset session

set datafile separator ','
set terminal pngcairo size 1920, 1080
set output "plot-full.png"
set grid
set xlabel "Time (s)"
set offset graph 0,0.1,0.1,0
set key font ",12"
set xtics font ",12"
set ytics font ",12"
set ylabel font ",16"
set xlabel font ",16"
set title font ",16"

set title "Queue Size"
set ylabel "Packets in Queue"
plot "output/receiver_data.csv" using 1:2 title "Queue Size" with lines lw 4

set title "Ratio of New and Retransmitted Packets in Queue"
set ylabel "Packet Ratio"
#plot "output/receiver_data.csv" using 1:3 title "Packet Ratio" with lines lw 4

set title "Useful Throughput of Receiver"
set ylabel "Goodput (%)"
#plot "output/receiver_data.csv" using 1:4 title "Goodput" with lines lw 4

#set title "Send Rates"
#plot for [i=0:(ARG1-1)] "output/sender_data".i.".csv" using 1:2 title "Sender-".i with lines lw 3

#set title "New / Total Packets Sent Out"
#plot for [i=0:(ARG1-1)] "output/sender_data".i.".csv" using 1:3 title "Sender-".i with lines lw 3