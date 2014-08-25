#! /bin/bash
time ./DUT.py  > >(tee stdout.log) 2> >(tee stderr.log >&2)
