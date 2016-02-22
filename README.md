# Train 1
A compiled version of the source code in this repository can be found at the
following URL:
```
/u/cs452/tftp/ARM/tpetrick/t1.elf
```
Alternatively, the source code can be compiled using GNU make by running
`make` in the top level directory (the same one as this README). This will
produce the ELF binary file `./bin/t1.elf`.

The binary can be loaded on the board by running the following command in
Redboot:
```
load -b 0x50000 -h 10.15.167.5 "/u/cs452/tftp/ARM/tpetrick/t1.elf"
```