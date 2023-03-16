#!/usr/bin/env python
import glob
import re
import datetime
import time
import math
import sys

DATE_REGEX = "(\d{4})_(\d{2})_(\d{2})*"

cur_time = datetime.datetime(
    *map(int, re.match(DATE_REGEX, time.strftime("%Y_%m_%d")).groups()))


files = glob.glob("*")
min_diff = math.inf
min_file = None
for file in files:
    if (m := re.match(DATE_REGEX, file)):
        d_time = datetime.datetime(*map(int, m.groups()))
        d = (cur_time - d_time).total_seconds()
        if d < min_diff:
            min_diff = d
            min_file = file

if min_file is not None:
    print(min_file)
else:
    print("unable to find directory in %Y_%m_%d format", file=sys.stderr)
    exit(1)
