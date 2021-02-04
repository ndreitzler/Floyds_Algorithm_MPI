#!usr/bin/env python3

import sys

fptr = open(sys.argv[-1], 'r')

for line in fptr:
    if "00" in line:
        print(line, end = '')
        