#!/usr/bin/env python3

print("ANSI")

for i in range(8):
    print("\x1b[%im%4i" % (i + 30, i + 30), end="\x1b[0m ")

print()

for i in range(8):
    print("\x1b[%im\x1b[30m%4i" % (i + 40, i + 40), end="\x1b[0m ")

print()

print("Extended")

for i in range(8):
    print("\x1b[%im%4i" % (i + 90, i + 90), end="\x1b[0m ")

print()

for i in range(8):
    print("\x1b[%im\x1b[30m%4i" % (i + 100, i + 100), end="\x1b[0m ")

print()

print("256")

for i in range(8):
    print("\x1b[48;5;%im\x1b[30m%4i" % (i, i), end="\x1b[0m ")

print()

for i in range(8, 16):
    print("\x1b[48;5;%im\x1b[30m%4i" % (i, i), end="\x1b[0m ")

print()
print()

for x in range(2):
    for y in range(6):
        for z in range(3):
            for w in range(6):
                i = w + (z + 3 * x) * 36 + 6 * y + 16
                print("\x1b[48;5;%im\x1b[30m%4i" %
                      (i, i), end="\x1b[0m")
            print("", end=" ")
        print()
    print()

for i in range(24):
    print("\x1b[48;5;%im\x1b[30m%4i" %
          (i + 232, i + 232), end="\x1b[0m")

print()
print()

ATTR_ON = [1, 4, 3, 9, 7, 2, 5, 21, 6]
ATTR_OFF = [22, 24, 23, 29, 27, 22, 25, 28, 25]

for i, on in enumerate(ATTR_ON):
    print("\x1b[%im%4i\x1b[0m" %
          (on, on), end="")

print()

for i, on in enumerate(ATTR_OFF):
    print("\x1b[%im%4i\x1b[0m" %
          (on, on), end="")

print()
