#!/bin/sh

norun=false

topologies='Mesh_XY NoI_Mesh NoI_CMesh'
chiplet_numbers='16 8 4 2 1'
injection_rates='0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08'
#injection_rates='0.001 0.002 0.003 0.004 0.005 0.006 0.007 0.008'

while [[ $# -gt 0 ]]
do
	case "$1" in
		-n|--norun)
			norun=true
			shift # past argument (or value)
			;;
		--injection_rate)
			topologies='Mesh_XY NoI_Mesh NoI_CMesh'
			chiplet_numbers='4'
			injection_rates='0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08'
			shift
			;;
		--chiplet_scaling)
			topologies='Mesh_XY NoI_Mesh NoI_CMesh'
			chiplet_numbers='16 8 4 2 1'
			injection_rates='0.03'
			shift
			;;
		*)    # unknown option
			echo ERROR: unknown option
			exit
			;;
	esac
done

run()
{
	local num_cpus=64
	local num_dirs=64
	local topology=$1
	local mesh_rows=8
	local num_chiplets=$2
	local injection_rate=$3
	local sim_cycles=2000000 # Previous work run or 2M cycles, --sim-cycles=2000000

	local output_dir=results/$topology-${num_chiplets}_chiplets-ir$injection_rate/m5out

	if [ $norun == true ]
	then # Print executing command only
		echo ./build/NULL/gem5.opt configs/my/garnet_synth_traffic.py \
			--num-cpus=$num_cpus \
			--num-dirs=$num_dirs \
			--sys-clock=1.8GHz \
			--ruby-clock=1.8GHz \
			--network=garnet2.0 \
			--topology=$topology \
			--mesh-rows=$mesh_rows \
			--num-chiplets=$num_chiplets \
			--sim-cycles=$sim_cycles \
			--synthetic=uniform_random \
			--injectionrate=$injection_rate \
			--vcs-per-vnet=4
	else
		./build/NULL/gem5.opt --outdir=$output_dir --redirect-stdout --redirect-stderr configs/my/garnet_synth_traffic.py \
			--num-cpus=$num_cpus \
			--num-dirs=$num_dirs \
			--sys-clock=1.8GHz \
			--ruby-clock=1.8GHz \
			--network=garnet2.0 \
			--topology=$topology \
			--mesh-rows=$mesh_rows \
			--num-chiplets=$num_chiplets \
			--sim-cycles=$sim_cycles \
			--synthetic=uniform_random \
			--injectionrate=$injection_rate \
			--vcs-per-vnet=4 >&/dev/null

		tail -1 $output_dir/simout | grep -v 'Exiting @ tick [0-9]\+ because Network Tester completed simCycles'
		if [ "$?" == "0" ]
		then
			echo "Something wrong!! output_dir:$output_dir"
		fi
	fi
}

for topology in $topologies
do
	for num_chiplets in $chiplet_numbers
	do
		for injection_rate in $injection_rates
		do
			run $topology $num_chiplets $injection_rate &
		done
	done
done

wait

