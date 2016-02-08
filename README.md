# Kernel 3
A compiled version of the source code in this repository can be found at the
following URL:
```
/u/cs452/tftp/ARM/tpetrick/k3.elf
```
Alternatively, the source code can be compiled using GNU make by running
`make` in the top level directory (the same one as this README). This will
produce the ELF binary file `./bin/k3.elf`.

The binary can be loaded on the board by running the following command in
Reboot:
```
load -b 0x00218000 -h 10.15.167.5 "/u/cs452/tftp/ARM/tpetrick/k3.elf"
```
It can then be run by issuing the `go` command. The test tasks will use
the clock server and notifier to test delay commands. It the dumps
performance data for all processes. The Idle Task is highlighted.

The expected output can be found in clockTestOutput.txt.

0     idle task
1     initial task
2     name server
3     clock server
4     clock notifier
5     performance task
6-9   delay test tasks
