# Kernel 4
A compiled version of the source code in this repository can be found at the
following URL:
```
/u/cs452/tftp/ARM/tpetrick/k4.elf
```
Alternatively, the source code can be compiled using GNU make by running
`make` in the top level directory (the same one as this README). This will
produce the ELF binary file `./bin/k4.elf`.

The binary can be loaded on the board by running the following command in
Redboot:
```
load -b 0x50000 -h 10.15.167.5 "/u/cs452/tftp/ARM/tpetrick/k3.elf"
```

It can then be run by issuing the `go` command. An interface similar to the A0 submission will appear, including a wall clock, sensor list and switch list. The percentage of time spent in the idle task will be printed underneath the clock. The following commands to control the trains can be entered:

* tr id speed   Set train id to speed 
* sw id S|C     Set switch id to Straight or Curved
* rv id         Reverse train id
* q             Quit the program

Sensor state is continuously polled and updates the UI every time a new sensor is triggered. The UI will also show the current state (S or C) of each switch.
