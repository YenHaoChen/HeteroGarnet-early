set terminal x11

set xlabel 'Injection rate'	# Put a label on the x-axis
set ylabel 'Average latency (ps)'	# Put a label on the y-axis

#set xrange [0.001:0.005]	# Change the x-axis range
#set yrange [20:500]	# Change the y-axis range
set autoscal	# Have Gnuplot determine ranges

#set logscale	# Plot using log-axes
#set logscale y	# Plot using log-axes on y-axis
#unset logscale

plot	'parse_ir-latency.out' using 1:2 title 'Mesh' with linespoints, \
	'parse_ir-latency.out' using 1:3 title 'NoI\_Mesh' with linespoints, \
	'parse_ir-latency.out' using 1:4 title 'NoI\_CMesh' with linespoints

