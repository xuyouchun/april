#! /usr/bin/python3

import sys
import os
import re

##-------- ---------- ---------- ---------- ----------

class CTransfer(object):

    def to_code(self, block):
        s = 'const char_t * %s = _T("' % block.name
        index = 0
        for line in block.lines:
            if index > 0:
                s += "\\n\\\n"
            s += self.__encode(line)
            index += 1
        s += '");\n'
        return s

    __escapes = [
        ("\\", r"\\"),
        ("\"", r"\""),
        ("\n", r"\n"),
        ("\r", r"\r"),
        ("\t", r"\t"),
        ("\b", r"\b"),
        ("\a", r"\a"),
        ("\v", r"\v"),
        ("\f", r"\f"),
    ]

    def join(self, codes):
        separator = "\n//-------- ---------- ---------- ---------- ----------\n\n"
        return separator.join(codes)

    def __encode(self, line):
        for s1, s2 in self.__escapes:
            line = line.replace(s1, s2)
        return line

##-------- ---------- ---------- ---------- ----------

class Block(object):

    def __init__(self):
        self.name = None
        self.lines = []

    def empty(self):
        return self.name is None

    def __str__(self):
        return '%s\n%s' % (self.name, '\n'.join(self.lines))

##-------- ---------- ---------- ---------- ----------

class Converter(object):

    def __init__(self, lines):
        self.__lines = lines
        self.__split_line = None
        self.__codes = []
        self.__transfer = CTransfer()


    def convert(self):
        for block in self.__read_blocks():
            self.__codes.append(self.__transfer.to_code(block))
        return self.__transfer.join(self.__codes)

    def __read_blocks(self):
        block = Block()
        lineno = 0
        for line in self.__lines:
            lineno += 1;
            if self.__split_line is None:
                if not _is_empty_line(line) and not _is_cmd_line(line):
                    self.__split_line = line
            elif line == self.__split_line:
                yield block
                block = Block()
            elif block.name is None:
                if _is_empty_line(line):
                    raise Exception("line: %s, format error: name is empty" % lineno)
                block.name = line
            else:
                block.lines.append(line)

        if not block.empty():
            yield block

##-------- ---------- ---------- ---------- ----------

def _is_empty_line(s):
    return re.match(r'^\s*$', s) is not None

def _is_cmd_line(s):
	return re.match(r'^\s#!', s) is not None

#////////// ////////// ////////// ////////// //////////

def __main():
    lines = [line.rstrip('\r\n') for line in sys.stdin.readlines()]
    converter = Converter(lines)
    print(converter.convert())

__main()

