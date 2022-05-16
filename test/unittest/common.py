
#! /usr/bin/python

import os
import sys
import re

import pytest

##############################################################################

""" Returns parent path """
def __get_parent_path(path, level = 1):
    for k in range(level):
        path = os.path.dirname(path)
    return path

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
        raise RuntimeError("code file " + test_file + " not exists")

    command = april_path + " run " + file + " " + get_absolute_path("System.apl")
    print(command)

    s = "".join(os.popen(command).readlines())

    return s


##############################################################################

class Base:

    def run(self, file):
        result, expect = self.__run(file);
        assert result == expect

    def __run(self, file):
        result = run_file(file)
        result = re.sub(r"^[\n\r]*|[\n\r]*$", "", result)

        expect = self.__read_expect(file).replace("\r", "")
        expect = re.sub(r"^[\n\r]*|[\n\r]*$", "", expect)

        if result != expect:
            if '\n' in expect or '\n' in result:
                print("EXPECT:\n" + expect)
                print("ACTUAL:\n" + result)
            else:
                print("EXPECT: " + expect)
                print("ACTUAL: " + result)

        return (result, expect)

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



