# Kernel 1
A compiled version of the source code in this repository can be found at the
following URL:
```
/u/cs452/tftp/ARM/tpetrick/main.elf
```
Alternatively, the source code can be compiled using GNU make by running
`make` in the top level directory (the same one as this README). This will
produce the ELF binary file `./bin/main.elf`.

The binary can be loaded on the board by running the following command in
Reboot:
```
load -b 0x00218000 -h 10.15.167.5 "/u/cs452/tftp/ARM/tpetrick/main.elf"
```
It can then be run by issuing the `go` command.

The expected output is:

```Created: 0x02                                                               
Created: 0x03
Tid: 0x04   Pid: 0x01
Tid: 0x04   Pid: 0x01
Created: 0x04
Tid: 0x05   Pid: 0x01
Tid: 0x05   Pid: 0x01
Created: 0x05
FirstUserTask: exiting
Tid: 0x02   Pid: 0x01
Tid: 0x03   Pid: 0x01
Tid: 0x02   Pid: 0x01
Tid: 0x03   Pid: 0x01
```
