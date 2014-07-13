
# Python for development testing

This directory contains python scripts for testing during code development.

These are not intended to be used as production software.

## DUT.py

Connect a device to the Default Port (usually /dev/ACM0) at the Default Baudrate (usually 115200). 

* Note: This program does not run well when other processes are accessing the same serial port.

## util.py

This is a file with some miscellaneous python functions, including nicer logging functions (info, error etc)


