#!/bin/sh

topologies='Mesh_XY NoI_Mesh NoI_CMesh'
injection_rates='0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08'
#injection_rates='0.001 0.002 0.003 0.004 0.005 0.006 0.007 0.008'

echo -n "injetion_rate(ps) "
for topology in $topologies
do
	echo -n "$topology "
done
echo

for injection_rate in $injection_rates
do
	echo -n "$injection_rate "
	for topology in $topologies
	do
		dir=results/$topology-${num_chiplets}_chiplets-ir$injection_rate/m5out
		file=$dir/stats.txt

		average_hops=`grep 'system.ruby.network.average_hops' $file | awk '{print $2}'`
		average_packet_latency=`grep 'system.ruby.network.average_packet_latency' $file | awk '{print $2}'`

	#	echo 'average_hops:' $average_hops
	#	echo 'average_packet_latency(ticks, ps):' $average_packet_latency

		echo -n "$average_packet_latency "
	done
	echo
done
