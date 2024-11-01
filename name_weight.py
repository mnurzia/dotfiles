#!/usr/bin/env python3
import sys

for name, w in sorted(
    map(lambda x: (x[0], sum(x[1]) / len(x[1])),
        map(lambda y:
            (y, [ord(c.lower()) - ord('a') for c in y.replace(" ", "")]),
            sys.argv[1:])),
    key=lambda x: x[1],
    reverse=True
):
    print(f"{w:02f}\t{name}")
