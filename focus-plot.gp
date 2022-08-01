reset session

set datafile separator ','
set terminal pngcairo size 1920, 1080
set output "plot-full.png"
set grid
set xlabel "Time (Simulator Ticks)"
set offset graph 0,0.1,0.1,0
set key font ",18"
set xtics font ",18"
set ytics font ",18" 
set ylabel font ",20" offset -2.5, 0
set xlabel font ",20"
set title font ",20"
set lmargin 13

set title "Queue Size"
set ylabel "Packets in Queue"
set key at graph 0.95, 1
plot "output/receiver_data.csv" using 1:2 title "Queue Size" with lines lw 5

set output "plot_full-2.png"

set key at graph 1, 1
set title "Ratio of New and Retransmitted Packets in Queue"
set ylabel "Packet Ratio"
plot "output/receiver_data.csv" using 1:3 title "Packet Ratio" with lines lw 5

set output "plot_full-3.png"

set title "Useful Throughput of Receiver"
set ylabel "Useful Throughput (%)"
plot "output/receiver_data.csv" using 1:4 title "Useful Throughput" with lines lw 5

#set title "Send Rates"
#plot for [i=0:(ARG1-1)] "output/sender_data".i.".csv" using 1:2 title "Sender-".i with lines lw 3

#set title "New / Total Packets Sent Out"
#plot for [i=0:(ARG1-1)] "output/sender_data".i.".csv" using 1:3 title "Sender-".i with lines lw 3