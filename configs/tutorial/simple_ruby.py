'''
Configuring a simple Ruby system - gem5 Tutorial 0.1 documentation
http://learning.gem5.org/book/part3/MSI/configuration.html

First, createa new configuration directory in configs/. Just like all gem5
confguration files, we will have a configuration run script. For the run
script, we can start with simple.py from "Creating a simple configuration
script. Copy the file to simple_ruby.py in your new directory.
We will make a couple of small changes to this file to use Ruby instead of
directly connecting the CPU to the memory controllers.
'''

import m5
from m5.objects import *

# Create the first SimObject: the system that we are going to simulate
system = System()

# Set the clock on the system
system.clk_domain = SrcClockDomain() # Create a clock domain
system.clk_domain.clock = '1GHz' # Set the clock frequency on that domain
system.clk_domain.voltage_domain = VoltageDomain() # Specify a voltage domain

# Always use timing mode for the memory simulation,
# except in special cases like fast-forwarding and restoring from checkpoint
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('512MB')]

# Use two CPUs. So, we can test our coherence protocol
system.cpu = [TimingSimpleCPU(), TimingSimpleCPU()]

# Create the system-wide memory bus
system.membus = SystemXBar()

#DELETE
# Connect the cache ports on the CPU to the memory bus
#system.cpu.icache_port = system.membus.slave
#system.cpu.dcache_port = system.membus.slave

# Connect up a few other ports
system.cpu.createInterruptController() # Create an I/O controller on the CPU
system.cpu.interrupts[0].pio = system.membus.master
system.cpu.interrupts[0].int_master = system.membus.slave
system.cpu.interrupts[0].int_slave = system.membus.master
system.system_port = system.membus.slave # Connect o the membus

# Use a simple DDR3 controller
system.mem_ctrl = DDR3_1600_8x8()
system.mem_ctrl.range = system.mem_ranges[0] # Create a memory controller
system.mem_ctrl.port = system.membus.master # Connect to the membus

# Use Ruby instead of directly connecting the CPU to the memory controller
system.caches = MyCacheSystem() # Create the cache system
system.caches.setup(system, system.cpu, [system.mem_ctrl]) # Set up all caches

# Set up the process we want the CPU to execute for sycall emulation mode
process = Process() # Create the process (another SimObject)
process.cmd = ['tests/test-progs/hello/bin/x86/linux/hello'] # Set the command
system.cpu.workload = process # Set the CPU to use the process as workload
system.cpu.createThreads() # Create the functional execution contexts

# Instantiate the system and begin execution
root = Root(full_system=False, system=system) # Create the Root object
m5.instantiate() # Instantiate the simulation

# Kick off the actual simulation!
print("Beginning simulation!") # gem5 uses Python 3-style print functions
exit_event = m5.simulate()

# Inspect the state of the system when simulation finishes
print('Exiting @ tick {} because {}'
                .format(m5.curTick(), exit_event.getCause()))

