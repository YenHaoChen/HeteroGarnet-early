#!/bin/sh

./build/NULL/gem5.debug configs/example/garnet_synth_traffic.py \
		--num-cpus=4 \
		--num-dirs=4 \
		--network=garnet2.0 \
		--topology=Mesh_XY \
		--mesh-rows=2 \
		--sim-cycles=100000 \
		--synthetic=uniform_random \
		--injectionrate=0.05 \
		--vcs-per-vnet=4


