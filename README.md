marionette
==========

**Configurable Data Acquisition Board Firmware**

* Processor: STM32F407

* OS: ChibiOS  https://github.com/ChibiOS/ChibiOS-RT

* Submodule: ChibiOS, branch 2.6.x

# Description

Firmware for an interactive configurable DAQ board.

# Interfaces

* GPIO
* ADC
* DAC
* SPI
* I2C

# Interaction

Interaction through the USB/ttyACMx serial device running in the ChibiOS shell.

## Tools

Linux tools include 'minicom' and 'gtkterm'.

Windows tools include *???*

# Building

## An ARM compiler must be installed.

```
	~  > arm-none-eabi-gcc --version
	arm-none-eabi-gcc (GNU Tools for ARM Embedded Processors) 4.8.3 20140228 (release) [ARM/embedded-4_8-branch revision 208322]
	Copyright (C) 2013 Free Software Foundation, Inc.
	This is free software; see the source for copying conditions.  There is NO
	warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```
### One source for ARM GCC cross compilers:

https://launchpad.net/gcc-arm-embedded

## Makefile build system

Change directory to the source in .../src/marionette.
To build type 'make' 

# Programming

## Dependencies

This programming method depends on the open source program Openocd.

http://openocd.sourceforge.net/

## Steps

Using an STLINK-V2 programmer
	type make write_stl

```

> make write_stl
	openocd -s ../../toolchain/openocd -f stlinkv2_stm32_e407.cfg -c "program build/ch.hex verify reset"
	Open On-Chip Debugger 0.8.0 (2014-04-29-15:41)
	Licensed under GNU GPL v2
	For bug reports, read
		http://openocd.sourceforge.net/doc/doxygen/bugs.html
	Info : This adapter doesn't support configurable speed
	Info : STLINK v2 JTAG v17 API v2 SWIM v0 VID 0x0483 PID 0x3748
	Info : using stlink api v2
	Info : Target voltage: 2.885361
	Info : stm32f4x.cpu: hardware has 6 breakpoints, 4 watchpoints
	target state: halted
	target halted due to debug-request, current mode: Thread 
	xPSR: 0x01000000 pc: 0x080001d0 msp: 0x20000400
	** Programming Started **
	auto erase enabled
	Info : device id = 0x10016413
	Info : flash size = 1024kbytes
	Info : Padding image section 0 with 4 bytes
	Info : Padding image section 1 with 4 bytes
	target state: halted
	target halted due to breakpoint, current mode: Thread 
	xPSR: 0x61000000 pc: 0x20000042 msp: 0x20000400
	wrote 49152 bytes from file build/ch.hex in 2.285748s (21.000 KiB/s)
	** Programming Finished **
	** Verify Started **
	target state: halted
	target halted due to breakpoint, current mode: Thread 
	xPSR: 0x61000000 pc: 0x2000002e msp: 0x20000400
	target state: halted
	target halted due to breakpoint, current mode: Thread 
	xPSR: 0x61000000 pc: 0x2000002e msp: 0x20000400
	target state: halted
	target halted due to breakpoint, current mode: Thread 
	xPSR: 0x61000000 pc: 0x2000002e msp: 0x20000400
	verified 46568 bytes in 0.475625s (95.614 KiB/s)
	** Verified OK **
	** Resetting Target **
	shutdown command invoked

```
Using an Olimex JTAG programmer
	type make write


