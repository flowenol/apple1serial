![apple1serial](/apple1serial.jpg)

# apple1serial

This repository contains firmware and gerber files for the Apple-1 Serial Interface expansion card.

## Description

This project aims to provide an alternative to the Apple Cassette Interface for the Apple-1 computer with use of the serial
communication (9600/8-N-1). The expansion card is based around inexpensive and easily available ATMega328P MCU which operates
as half-duplex UART. The signal levels are TTL, so adapter boards may be required if you need some other standard. The best way
to connect the device to a regular modern PC is to use FTDI FT232RL USB dongle.

The board tries to mimic the original ACI behaviour via the modified ACI loader program `$C100` located on the on-board ROM.
The usual way to communicate with the device is to hook it up to a PC and use a terminal emulator program to send and
receive binary files representing the Apple-1 memory contents. An alternative is to use custom software/scripts like the
`scripts/transfer.py` python script.

In order to learn how to use the loader program, please refer to the original ACI documentation or the instructions provided [here](https://www.sbprojects.net/projects/apple1/aci.php).

Some Apple-1 program binaries ready to be downloaded are available [here](https://drive.google.com/file/d/1G0ycKSszlr45RE8Rp6eW-0qxz4MS9qDN/view?usp=sharing).

## Memory map

| address | function |
| --- | --- |
| `$C0FC-$C0FF` | these addresses contain the 'A1SI' string which might be used to determine whether the A1SI expansion card is present |
| `$C100-$C7FF` | address space reserved for on-board ROM |
| `$C100` | the original ACI loader program adapted to the A1SI |
| `$C300` | "teletype on apple-1" test program - the data from remote terminal appears on Apple-1 screen |
| `$C400` | "teletype on remote" test program - the data from Apple-1 appears on remote terminal |
| `$C500` | 0-255 repeating counter - all possible bytes are repeatedly sent to remote terminal |
| `$C600` | serialmonitor - the well known Woz Monitor variation, but now on the remote terminal |

entries for programmers:

| address | function |
| --- | --- |
| `$C000` | ready flag - contains 0x01 if the device is ready to write/read, 0x00 otherwise |
| `$C080` | read address - the location for data read from the interface, first read of this address switches the device to read mode |
| `$C081` | write address - the location for data to be written via the interface, first write to this address switches the device to write mode |
| `$C23A` | software reset routine - resets the MCU |
| `$C1EC` | block read routine - reads data from the interface and writes to the specified address range |
| `$C213` | block write routine - reads the data from the specified address range and write via the interface |
| `$0034` | lower byte of the end address for block read/write routines |
| `$0035` | upper byte of the end address for block read/write routines |
| `$0036` | lower byte of the beginning address for block read/write routines |
| `$0037` | upper byte of the beginning address for block read/write routines |

## Contents

The contents of this repository are as following:

* gerber/ - gerber files needed to manufacture the PCB
* firmware/ - contains source code for the ATMega328 MCU firmware
* mapping/ - contains EQN and JED files for GAL20V8 based address decoder for the board
* src/ - contains the 6502 assembly sources for the A1SI onboard ROM routines
* scripts/ - contains python based file transfer utility

## Requirements

You need the following to successfully build the firmware:

* [xa](https://www.floodgap.com/retrotech/xa/) cross assembler
* [avr-gcc](https://gcc.gnu.org/wiki/avr-gcc) toolchain
* [avr-libc](https://www.nongnu.org/avr-libc/) set of libraries
* [AVRDUDE](https://www.nongnu.org/avrdude/) AVR programmer software or equivalent to program the ATMega328P MCU
* Software capable of translating the .eqn files into .jed's. I used for this purpose the DOS based EQN2JED from OPALjr PLD Development Package. This is only required if you wish to make some changes to the GAL based address decoder. The default .jed file should be fine for most users.
* EEPROM programmer to write ROM and GAL. I used [TL866](http://autoelectric.cn/EN/TL866_main.html) programmer for this purpose. It can also program the ATMega328P MCU.

## How to build?

To build the firmware just type:

`make`

And to clean the build:

`make clean`

## Applesoft BASIC support

You can also `LOAD` and `SAVE` your Applesoft BASIC programs thanks to the branch of the
applesoft-lite project which has been modified to be compatible with the A1SI expansion card:

http://github.com/flowenol/applesoft-lite

## How to install hardware?

Solder the "C" memory segment line near 74154 demultiplexer to the "T" signal line.
Afterwards just put the board in right orientation (as marked on the PCB) in the Apple-1 expansion slot.
Or you can use the port expander if the on-board slot is already occupied:

https://github.com/flowenol/Apple1ExpanderPcb

## PCB

Gerber files can be found in the `gerber/` directory.
The KiCad project files with board design and schematics can be found here:

http://github.com/flowenol/Apple1SerialPcb


## Bill of materials

| name | part | quantity |
| --- | --- | --- |
| C1, C2  | Ceramic 22pF  | 2 |
| C3..C9  | Ceramic 100nF | 7 |
| C10     | Electrolytic 22uF | 1 |
| R1, R2  | 0.68K 0.25W    | 2 |
| R3..R9  | 1K 0.25W       | 7 |
| XTAL1   | 16Mhz crystal HC49 low profile | 1 |
| ATmega328P1 | ATMega328P microcontroller DIP28 | 1 |
| 7402 | 7402 quad 2 input NOR gates DIP14 | 1 |
| 74S257mux1,2 | 74S257 quad multiplexers DIP16 | 2 |
| 8T97buf1,2   | 8T97N hex tristate buffers DIP16 | 2 |
| 20V8B1  | GAL20V8 SPLD (35ns or less) DIP24 | 1 |
| AT28C64B1 | AT28C64 ROM or similar DIP-28 | 1 |
| LED1 | LM358N double op amp DIP8 | 1 |
| TX_LED1, RX_LED1 | LED phi 3mm | 2 |
| L7805 | LM7805 regulator (optional) | 1 |
| SERIAL1 | 1x3 2.54mm pin header | 1 |
| ROM_PWR1| 1x3 2.54mm pin header (optional -  make solder blob to "5V" instead) | 1 |
