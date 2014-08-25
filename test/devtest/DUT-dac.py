#!/usr/bin/env python
# file: DUT.py


"""
Sun 13 July 2014 11:55:14 (PDT)
TODO: Python exception handling with the serial port is not great.
    - minicom might be open and accessing the port at the same time but exceptions aren't
        consistently raised in serial.read/serial.readline

NOTE: ctrl-c quitting-may take more than one ctrl-c for program to exit
"""


""" 
This tool is used to manipulate a D_evice U_nder T_est

Example: 

Connect a device to the Default_Port.
~/.../library/python/devtest (adc*) > ./DUT.py 

"""

import sys
import os
import threading
import serial
from time import sleep
import utils as u

DUT_DAC_SLEEP     = 0.750
DUT_SERIAL_WAKEUP     = 2
DUT_WAITTIME     = 0.200
Default_Baudrate = 115200
Default_Timeout  = 2
Default_Port     = "/dev/ttyACM0"

class DUTSerial():
    def __init__(self, port=Default_Port, baud=Default_Baudrate, timeout=Default_Timeout):
        self.serial_port    = port
        self.baud           = baud
        self.timeout        = timeout
        self.isOpen         = False
        self.Totallines     = 0
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
                if os.path.exists(self.ser.port)!=True:
                    u.info("Pausing for serial")
                    sleep(DUT_SERIAL_WAKEUP/2)
                sleep(DUT_SERIAL_WAKEUP/2)
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
                    self.Totallines += 1
                else:
                    pass
            print("")
            sys.stdout.flush()
        except serial.SerialException  as e:
            self.alive = False
            u.error("Error reading serial port: " + str(e))
            self.close()
            sys.exit()

    def teststr(self, string, echo=False):
        if echo==True:
            u.info("sending\t->"+string)
        self.write(string)
        sleep(DUT_WAITTIME)

    def test_dac(self):
        try:
            for i in range(0,1):
#               response = input("test one shot with default profile(Y/n) ")
#               if response!='y':
                  self.teststr("dac:vref_mv(3300)\r\n", True)
                  self.teststr("dac:conf_ch1:dc_mv(1000)\r\n", True)
                  sleep(DUT_DAC_SLEEP)
                  self.teststr("dac:conf_ch1:dc_mv(750)\r\n", True)
                  sleep(DUT_DAC_SLEEP)
                  self.teststr("dac:conf_ch1:dc_mv(20)\r\n", True)
                  sleep(DUT_DAC_SLEEP)
                  self.teststr("dac:conf_ch1:dc_mv(0)\r\n", True)
                  sleep(DUT_DAC_SLEEP)
                  self.teststr("dac:conf_ch1:dc_mv(3301)\r\n", True)
                  sleep(DUT_DAC_SLEEP)
                  self.teststr("dac:conf_ch2:dc_mv(1000)\r\n", True)
                  sleep(DUT_DAC_SLEEP)
                  self.teststr("dac:conf_ch2:dc_mv(750)\r\n", True)
                  sleep(DUT_DAC_SLEEP)
                  self.teststr("dac:conf_ch2:dc_mv(20)\r\n", True)
                  sleep(DUT_DAC_SLEEP)
                  self.teststr("dac:conf_ch2:dc_mv(0)\r\n", True)
                  sleep(DUT_DAC_SLEEP)
                  self.teststr("dac:conf_ch2:dc_mv(3301)\r\n", True)
                  sleep(DUT_DAC_SLEEP)

                  u.info("Negative Voltage request should produce an error")
                  self.teststr("dac:conf_ch1:dc_mv(-20)\r\n", True)

                  self.teststr("dac:stop\r\n",0)
                  self.teststr("dac:start\r\n",0)
                  sleep(DUT_DAC_SLEEP)
                  self.teststr("dac:release\r\n",0)
                  sleep(DUT_DAC_SLEEP)
                  self.teststr("dac:stop\r\n",0)
                  sleep(DUT_DAC_SLEEP)
                  self.teststr("dac:start\r\n",0)
        except KeyboardInterrupt:
            self.close()
            u.info("\nQuitting")

 
    def writer(self):
        try:
            if self.alive:
                self.teststr("+noprompt\r\n")
                self.test_dac()
                self.teststr("+prompt\r\n")
        except:
            self.alive = False
            self.close()
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
        #baud           = 460800
        baud           = 115200
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

