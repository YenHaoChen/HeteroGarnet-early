set terminal x11

set xlabel 'Number of chiplets'	# Put a label on the x-axis
set ylabel 'Average latency (ps)'	# Put a label on the y-axis

#set xrange [0.001:0.005]	# Change the x-axis range
#set yrange [20:500]	# Change the y-axis range
set autoscal	# Have Gnuplot determine ranges

set logscale x	# Plot using log-axes on x-axis
#set logscale y	# Plot using log-axes on y-axis
#unset logscale

#set xtics (1,2,4,8,16,32,64,128,256,512,1024)

plot	'parse_chiplet_scaling.out' using 1:2 title 'Mesh' with linespoints, \
	'parse_chiplet_scaling.out' using 1:3 title 'NoI\_Mesh' with linespoints, \
	'parse_chiplet_scaling.out' using 1:4 title 'NoI\_CMesh' with linespoints

