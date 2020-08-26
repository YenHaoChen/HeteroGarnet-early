#!/bin/sh

topologies='Mesh_XY NoI_Mesh NoI_CMesh'
chiplet_numbers='16 8 4 2 1'
injection_rates='0.03'

echo -n "chiplet_numbers(ps) "
for topology in $topologies
do
	echo -n "$topology "
done
echo

for num_chiplets in $chiplet_numbers
do
	echo -n "$num_chiplets "
	for topology in $topologies
	do
		for injection_rate in $injection_rates
		do
			dir=results/$topology-${num_chiplets}_chiplets-ir$injection_rate/m5out
			file=$dir/stats.txt

			average_hops=`grep 'system.ruby.network.average_hops' $file | awk '{print $2}'`
			average_packet_latency=`grep 'system.ruby.network.average_packet_latency' $file | awk '{print $2}'`

		#	echo 'average_hops:' $average_hops
		#	echo 'average_packet_latency(ticks, ps):' $average_packet_latency

			echo -n "$average_packet_latency "
		done
	done
	echo
done
