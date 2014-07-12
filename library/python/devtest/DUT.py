#!/usr/bin/env python
# file: DUT.py

""" 
This tool is used to manipulate a D_evice U_nder T_est

Example Usage:

~/.../library/python/devtest (python*) > ./DUT.py 

*** INFO:  opened port /dev/ttyACM0

--- sent->	 gpio:configure:porth:pin2:output:floating

--- sent->	 gpio:set:porth:pin2

--- sent->	 gpio:get:porth:pin2

--- sent->	 gpio:clear:porth:pin2

--- sent->	 gpio:get:porth:pin2

--- sent->	 gpio:set:porth:pin2

--- sent->	 resetpins

--- sent->	 gpio:get:porth:pin2


*** INFO:  closed port: /dev/ttyACM0

Bye.
~/.../library/python/devtest (python*) > 

"""

import sys
from time import sleep
import serial
import utils as u

#Exit_Character = serial.to_bytes([0x1d])   # CTRL+]

Default_Baudrate = 115200
Default_Timeout  = 2
Default_Port     = "/dev/ttyACM0"

class DUTSerial():
    def __init__(self, port=Default_Port, baud=Default_Baudrate, timeout=Default_Timeout):
        self.serial_port    = port
        self.baud           = baud
        self.timeout        = timeout
        self.isOpen         = False
        return

    def start(self):
        if not self.isOpen:
            self.ser          = serial.Serial()
            self.ser.baudrate = self.baud
            self.ser.port     = self.serial_port
            self.ser.timeout  = self.timeout

            """
             pyserial can't always grab the device. Change something trivial
             Then it works. 
            """
            self.ser.parity   = serial.PARITY_ODD
            self.ser.open()
            self.ser.close()
            self.ser.parity   = serial.PARITY_NONE
            self.ser.open()
            if self.ser.isOpen() == True:
                s = "opened port {}\n".format(self.serial_port)
                u.info(s)
                self.isOpen       = True
            else:
                print("Failed to open port: ", self.serial_port)
                self.isOpen       = False
        return

    def read(self):
        output = self.ser.readline(150) 
        return output

    def write(self, s):
        bytes = str.encode(s)    # Because python and strings
        numbytes = self.ser.write(bytes) 
        sys.stdout.write('--- sent->\t ')
        sys.stdout.write(s)
        sys.stdout.write('\n')
        sys.stdout.flush()
        return numbytes

    def close(self):
        if self.ser.isOpen() == True:
            self.ser.close()
            s="closed port: {}\n".format(self.serial_port)
            u.info(s)
        self.isOpen = False
        return

def DUT_capture(boardout):
    line = boardout.read()
    if len(line) > 0:
        print(line.decode('ascii'))
    else:
        print('*')
    return

def DUT_write(boardout, s):
    numbytes = boardout.write(s);
#    print(numbytes)
    return

if __name__ == "__main__":
    try:
        # optparse here  baud port filename quiet
        serial_port    = Default_Port
        baud           = Default_Baudrate
        timeout        = Default_Timeout

        #ser = DUTSerial(baud=baud, timeout=timeout)   # example of default args to init
        boardout       = DUTSerial(serial_port, baud, timeout)
        boardout.start()

        DUT_write(boardout, "gpio:configure:porth:pin2:output:floating\r\n")
        sleep(0.200)
        DUT_write(boardout, "gpio:set:porth:pin2\r\n")
        sleep(1.000)
        DUT_write(boardout, "gpio:get:porth:pin2\r\n")
        sleep(1.000)
        DUT_write(boardout, "gpio:clear:porth:pin2\r\n")
        sleep(1.000)
        DUT_write(boardout, "gpio:get:porth:pin2\r\n")
        sleep(1.000)
        DUT_write(boardout, "gpio:set:porth:pin2\r\n")
        sleep(1.000)
        DUT_write(boardout, "resetpins\r\n")
        sleep(1.000)
        DUT_write(boardout, "gpio:get:porth:pin2\r\n")
        sleep(1.000)

        #while True:
            #DUT_capture(boardout)

        boardout.close()
        print("Bye.")

    except serial.SerialException:
        boardout.close()
        print ('SerialException occurred')

    except KeyboardInterrupt:
        boardout.close()
        print ("\nQuitting")

# Attic

#      from time import sleep
#                self.ser.setDTR(True)
#                self.ser.setRTS(True)
#                sleep(0.100)
#      self.ser.xonxoff  = 1
#      self.ser.rtscts   = 0
#      self.ser.dsrdtr   = 0


