# Kernel 2
A compiled version of the source code in this repository can be found at the
following URL:
```
/u/cs452/tftp/ARM/tpetrick/k2.elf
```
Alternatively, the source code can be compiled using GNU make by running
`make` in the top level directory (the same one as this README). This will
produce the ELF binary file `./bin/k2.elf`.

The binary can be loaded on the board by running the following command in
Reboot:
```
load -b 0x00218000 -h 10.15.167.5 "/u/cs452/tftp/ARM/tpetrick/k2.elf"
```
It can then be run by issuing the `go` command.

Running the program will start a series of Rock Paper Scissors games. The games
can be advanced by pressing any key, such as the enter key. The expected output
can be found in rpsOutput.txt