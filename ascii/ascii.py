#!/usr/bin/env python3
COLS = 4

CTRLS = [
    "\\0",
    "SOH",
    "STX",
    "ETX",
    "EOT",
    "ENQ",
    "ACK",
    "\\a",
    "\\b",
    "\\t",
    "\\n",
    "\\v",
    "\\f",
    "\\r",
    "SO",
    "SI",
    "DLE",
    "DC1",
    "DC2",
    "DC3",
    "DC4",
    "NAK",
    "SYN",
    "ETB",
    "CAN",
    "EM",
    "SUB",
    "\\e",
    "FS",
    "GS",
    "RS",
    "US"
]
print("\x1b[1m " + " | ".join(["DEC OCT HEX CHR"] * 4) + "\x1b[0m")
for r in range(128 // COLS):
    for c in range(COLS):
        codep = r + c * (128 // COLS)
        pname = CTRLS[codep] if codep < len(
            CTRLS) else "DEL" if codep == 0x7F else chr(codep)
        print(" %3i %03o %02Xh %-3s " %
              (codep, codep, codep, pname), end="|" if c < COLS - 1 else "")
    print()
print("\x1b[1m " + " | ".join(["DEC OCT HEX CHR"] * 4) + "\x1b[0m")
