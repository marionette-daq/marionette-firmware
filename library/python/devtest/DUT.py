#!/usr/bin/env python
# file: DUT.py

""" 
This tool is used to manipulate a D_evice U_nder T_est

Example: 

~/.../library/python/devtest (python*) > ./DUT.py 

*** INFO:  opened port /dev/ttyACM0



m > +noprompt

gpio:configure:porth:pin2:output:floating

        DEBUG:  ../../src/fetch/gpio.c:297:gpio_config()->pin: 2

        DEBUG:  ../../src/fetch/gpio.c:298:gpio_config()->port: 0x40021C00 0x40020000

        DEBUG:  ../../src/fetch/gpio.c:299:gpio_config()->dir: 1

        DEBUG:  ../../src/fetch/gpio.c:300:gpio_config()->sense: 0

gpio:configure:porth:pin2:output:floating

        DEBUG:  ../../src/fetch/gpio.c:297:gpio_config()->pin: 2

        DEBUG:  ../../src/fetch/gpio.c:298:gpio_config()->port: 0x40021C00 0x40020000

        DEBUG:  ../../src/fetch/gpio.c:299:gpio_config()->dir: 1

        DEBUG:  ../../src/fetch/gpio.c:300:gpio_config()->sense: 0

gpio:set:porth:pin2

gpio:get:porth:pin2

1

gpio:clear:porth:pin2

gpio:get:porth:pin2

0

gpio:set:porth:pin2

resetpins

        DEBUG:  ../../src/fetch/fetch.c:155:fetch_resetpins()->Resetting pins

gpio:get:porth:pin2

0

+prompt

m > 
Bye.

"""

import sys
import threading
import serial
from time import sleep
import utils as u

DUT_WAITTIME     = 0.100
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
            self.ser.port     = self.serial_port   # readline and readlines will block forever if no timeout
            self.ser.timeout  = self.timeout

            """
             pyserial can't always grab the device. Change something trivial
             Then it works. 
            """
            self.ser.parity   = serial.PARITY_ODD
            try:
                self.ser.open()
            except serial.SerialException  as e:
                u.error("Error opening serial port: " + str(e))
                sys.exit()
            self.ser.close()
            self.ser.parity   = serial.PARITY_NONE
            self.ser.open()
            if self.ser.isOpen() == True:
                s = "opened port {}\n".format(self.serial_port)
                u.info(s)
                self.isOpen       = True
                self.alive        = True
                self._start_reader()
                self.tx_thread = threading.Thread(target=self.writer)
                self.tx_thread.setDaemon(True)
                self.tx_thread.start()

            else:
                print("Failed to open port: ", self.serial_port)
                self.isOpen       = False
                self.alive        = False
        return

    def join(self, tx_only=False):
        self.tx_thread.join()
        if not tx_only:
            self._reader_alive = False
            self.rx_thread.join()

    def _start_reader(self):
        """Start reader thread"""
        self._reader_alive = True
        self.rx_thread = threading.Thread(target=self.reader)
        self.rx_thread.setDaemon(True)
        self.rx_thread.start()

    def reader(self):
        try:
            while self.alive and self._reader_alive:
                line = self.ser.readline()    # don't forget timeout setting
                if len(line) > 0:
                    print(line.decode('ascii'), end="", flush=True)
                else:
                    pass
            print("")
            sys.stdout.flush()
        except serial.SerialException  as e:
            self.alive = False
            u.error("Error reading serial port: " + str(e))
            sys.exit()

    def teststr(self, string):
        u.info("sending\t->"+string)
        self.write(string)
        sleep(DUT_WAITTIME)

    def writer(self):
        try:
            if self.alive:
                self.teststr("")
                self.teststr("\r\n")
                self.teststr('\x04')   # ctrl-d in ascii will cause logout event, and marionette terminal will restart
                self.teststr("+noprompt\r\n")
                self.teststr("gpio:configure:porth:pin2:output:floating\r\n")
                self.teststr(" gpio : \tconfigure :p   orth:p\tin2:output:floa\t  \tt\t i n    g\r\n")
                self.teststr(" gpio \r\n")
                self.teststr("gpio:configure\r\n")
                self.teststr("gpio:configure:porti:pin10:output:floating\r\n")
                self.teststr("gpio:configure:porth:pin2:output:floating\r\n")
                self.teststr("gpio:set:porth:pin2\r\n")
                self.teststr("gpio:set:porti:pin10\r\n")
                self.teststr("gpio:get:porth:pin2\r\n")
                self.teststr("gpio:clear:porth:pin2\r\n")
                self.teststr("gpio:get:porth:pin2\r\n")
                self.teststr("gpio:get:porti:pin10\r\n")
                self.teststr("gpio:set:porth:pin2\r\n")
                self.teststr("resetpins\r\n")
                self.teststr("gpio:get:porth:pin2\r\n")
                self.teststr("+prompt\r\n")
        except:
            self.alive = False
            raise

    def write(self, s):
        bytes = str.encode(s)    # Because python and strings
        numbytes = self.ser.write(bytes) 
        return numbytes

    def close(self):
        self.join(True)
        self.join()
        self.alive  = False
        if self.ser.isOpen() == True:
            self.ser.close()
            self.isOpen = False
            s="closed port: {}\n".format(self.serial_port)
            u.info(s)
        return

if __name__ == "__main__":
    try:
        # optparse here eventually... baud port filename quiet
        serial_port    = Default_Port
        baud           = Default_Baudrate
        timeout        = Default_Timeout

        DUT       = DUTSerial(serial_port, baud, timeout)
        DUT.start()

        DUT.close()
        print("Bye.")

    except serial.SerialException:
        DUT.close()
        u.error("Serial Exception: " + str(e))
    except KeyboardInterrupt:
        DUT.close()
        u.info("\nQuitting")

# Attic

#    ser = DUTSerial(baud=baud, timeout=timeout)   # example of default args to init
#      from time import sleep
#                self.ser.setDTR(True)
#                self.ser.setRTS(True)
#                sleep(0.100)
#      self.ser.xonxoff  = 1
#      self.ser.rtscts   = 0
#      self.ser.dsrdtr   = 0

