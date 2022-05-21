
#! /usr/bin/python

import os
import sys
import subprocess
import re

import pytest

##############################################################################

""" Returns parent path """
def __get_parent_path(path, level = 1):
    for k in range(level):
        path = os.path.dirname(path)
    return path

def colorize(num, string, bold=False, highlight=False):
    attr = []

    if highlight:
        num += 10;

    attr.append(str(num))

    if bold:
        attr.append('1')

    return '\x1b[%sm%s\x1b[0m' % (';'.join(attr), string)

##############################################################################

""" Returns april root path """
def get_root_path():
    return __get_parent_path(__file__, 3)

def get_absolute_path(path):
    return os.path.join(get_root_path(), path)

""" Returns april bin file path """
def get_april_path():
    return get_absolute_path("bin/april")

""" Returns the path that contains test files """
def get_test_path():
    return get_absolute_path("test/unittest")

""" Runs a code file or project file or solution file """
def run_file(file):
    april_path = get_april_path()
    test_file = os.path.join(get_test_path(), file)

    if not os.path.exists(test_file):
        raise RuntimeError("Code file " + test_file + " not exists")

    command = april_path + " run " + file + " " + get_absolute_path("System.apl")
    print(colorize(36, command))

    code, ret = subprocess.getstatusoutput(command)
    if code != 0:
        raise RuntimeError("Status code is " + str(code))
        
    return ret


##############################################################################

class Base:

    def run(self, file):
        self.__run(file);

    def __run(self, file):
        result = run_file(file)
        result = re.sub(r"^[\n\r]*|[\n\r]*$", "", result)

        expect = self.__read_expect(file).replace("\r", "")
        expect = re.sub(r"^[\n\r]*|[\n\r]*$", "", expect)

        results = re.split(r'[\r\n]+', result);
        expects = re.split(r'[\r\n]+', expect);

        succeed = True
        if len(results) == len(expects):
            for k in range(len(results)):
                if results[k] != results[k]:
                    succeed = False
                    break

        if not succeed:
            if '\n' in expect or '\n' in result:
                print(colorize(35, "EXPECT:\n") + expect)
                print(colorize(35, "ACTUAL:\n") + result)
            else:
                print(colorize(35, "EXPECT: ") + expect)
                print(colorize(35, "ACTUAL: ") + result)

            raise RuntimeError('result is unexpected')


    def __read_expect(self, file):

        matched = False
        expect_results = []

        with open(file, "r") as file:
            while True:
                s = file.readline()
                if s == "":
                    break

                if re.match(r"\s*/\*\s*EXPECT", s):
                    matched = True
                    continue

                if matched:
                    if re.match(r"\s*\*/", s):
                        break

                    expect_results.append(s)

        assert matched
        return "".join(expect_results);



