#!/bin/sh

file=m5out/stats.txt

average_hops=`grep 'system.ruby.network.average_hops' $file | awk '{print $2}'`
average_packet_latency=`grep 'system.ruby.network.average_packet_latency' $file | awk '{print $2}'`

echo 'average_hops:' $average_hops
echo 'average_packet_latency(ticks):' $average_packet_latency
