#! /usr/bin/python

# Generates core_lib/System/Text/gb18030.bin, the table of System.Text.GB18030Encoding, from the gb18030 codec of Python
# (GB 18030-2005). The file is committed: the build does not run this script.
#
# The file (little endian):
#
#   Int32   n                 the number of the codes of two bytes (23940 = 126 * 190)
#   UInt16  n * code point    the characters of the codes of two bytes: the lead byte 0x81 to 0xFE, the trail byte
#                             0x40 to 0x7E and 0x80 to 0xFE (190 values); index = (lead - 0x81) * 190 + (trail < 0x80?
#                             trail - 0x40 : trail - 0x41)
#   Int32   m                 the number of the ranges of four bytes of the basic plane
#   m *     (Int32 linear start, Int32 code point start, Int32 count)
#                             the codes of four bytes b1 b2 b3 b4 have the number
#                             (((b1 - 0x81) * 10 + b2 - 0x30) * 126 + b3 - 0x81) * 10 + b4 - 0x30; the numbers
#                             linear start to linear start + count - 1 are the characters from code point start. The
#                             ranges are in the order of the numbers.
#
# The characters over U+FFFF are the codes of four bytes from 0x90308130 (the number 189000): U+10000 + number - 189000.

import os
import struct
import sys


def linear(code):
    b1, b2, b3, b4 = code
    return (((b1 - 0x81) * 10 + b2 - 0x30) * 126 + b3 - 0x81) * 10 + b4 - 0x30


def main():
    two = []
    for lead in range(0x81, 0xFF):
        for trail in list(range(0x40, 0x7F)) + list(range(0x80, 0xFF)):
            text = bytes([lead, trail]).decode("gb18030")
            assert len(text) == 1 and ord(text) <= 0xFFFF, (lead, trail)
            two.append(ord(text))

    assert len(two) == 126 * 190

    # The characters of the basic plane that are four bytes.
    four = []
    for cp in range(0x10000):
        if 0xD800 <= cp <= 0xDFFF:
            continue

        code = chr(cp).encode("gb18030")
        if len(code) == 4:
            four.append((linear(code), cp))

    four.sort()

    ranges = []
    for lin, cp in four:
        if ranges and lin == ranges[-1][0] + ranges[-1][2] and cp == ranges[-1][1] + ranges[-1][2]:
            ranges[-1][2] += 1
        else:
            ranges.append([lin, cp, 1])

    # The supplementary planes are linear.
    for cp in (0x10000, 0x10001, 0x1F600, 0x10FFFF):
        code = chr(cp).encode("gb18030")
        assert len(code) == 4 and linear(code) == 189000 + cp - 0x10000, hex(cp)

    # A character is two bytes or four bytes, not both, and the ranges of the codes and of the characters grow together.
    assert not set(two) & set(cp for lin, cp in four)
    assert all(ranges[k][1] < ranges[k + 1][1] and ranges[k][0] + ranges[k][2] <= ranges[k + 1][0] for k in range(len(ranges) - 1))

    data = struct.pack("<i", len(two)) + b"".join(struct.pack("<H", cp) for cp in two)
    data += struct.pack("<i", len(ranges)) + b"".join(struct.pack("<iii", *r) for r in ranges)

    path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "core_lib", "System", "Text", "gb18030.bin")
    with open(path, "wb") as f:
        f.write(data)

    print("two bytes: %d, four bytes of the basic plane: %d characters in %d ranges, file: %d bytes"
          % (len(two), len(four), len(ranges), len(data)))


if __name__ == "__main__":
    sys.exit(main())
