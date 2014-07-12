#!/usr/bin/env python

# 
# Miscellaneous utilities

# get division operator '/' vs. '//'
# (e.g., 1/2 == 0.5; 1//2 == 0)
from __future__ import division

__author__  = 'K Wilson'
__version__ = "0.0.1"

def get_hex(s):
    v = str(int(s,16))
    return v

def C_to_F(dC):
    """ convert Celcius to Fahrenheit """
    dC      = float(dC)
    dF      = (dC * (9.0/5.0)) + 32
    return dF

def twos_comp(rawval, nbits):
    """compute the 2's compliment in n bits"""
    rawval = int(rawval)
    if( (rawval&(1<<(nbits-1))) != 0 ):
        rawval = rawval - (1<<nbits)
    return rawval

def key_description(character):
    """generate a readable description for a key"""
    ascii_code = ord(character)
    if ascii_code < 32:
        return 'Ctrl+%c' % (ord('@') + ascii_code)
    else:
        return repr(character)

# Example
# import utils as u 
#  s = "opened port {}\n".format(self.serial_port,)
#  u.info(s)
def info(s):
    print("\n*** INFO: ",s)

def warning(s):
    print("\n*** WARNING: ",s)

def error(s):
    print("\n*** ERROR: ",s)

