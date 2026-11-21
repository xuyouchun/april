#! /usr/bin/python

# Generates core_lib/System/Globalization/unicode_category.bin, the table of System.Globalization.CharUnicodeInfo, from
# the unicodedata module of Python (the version of Unicode is printed, and written in the comment of CharUnicodeInfo).
# The file is committed: the build does not run this script.
#
# The file (little endian):
#
#   Int32   n               the number of the runs
#   UInt32  n * entry       a run of the code points that have the same category: (start << 5) | category, where the
#                           category is the number of System.Globalization.UnicodeCategory (0 to 29). The runs cover
#                           U+0000 to U+10FFFF, in the order of the start; a run ends where the next one starts.

import os
import struct
import sys
import unicodedata

# The order of System.Globalization.UnicodeCategory (the one of .NET).
CATEGORIES = [
    "Lu", "Ll", "Lt", "Lm", "Lo",
    "Mn", "Mc", "Me",
    "Nd", "Nl", "No",
    "Zs", "Zl", "Zp",
    "Cc", "Cf", "Cs", "Co",
    "Pc", "Pd", "Ps", "Pe", "Pi", "Pf", "Po",
    "Sm", "Sc", "Sk", "So",
    "Cn",
]


def main():
    number = {name: index for index, name in enumerate(CATEGORIES)}

    runs = []
    previous = -1
    for code in range(0x110000):
        category = number[unicodedata.category(chr(code))]
        if category != previous:
            runs.append((code << 5) | category)
            previous = category

    out = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "core_lib", "System", "Globalization",
                       "unicode_category.bin")

    with open(out, "wb") as f:
        f.write(struct.pack("<i", len(runs)))
        for entry in runs:
            f.write(struct.pack("<I", entry))

    print("Unicode %s: %d runs, %d bytes -> %s" % (unicodedata.unidata_version, len(runs), 4 + 4 * len(runs), out))
    return 0


if __name__ == "__main__":
    sys.exit(main())
