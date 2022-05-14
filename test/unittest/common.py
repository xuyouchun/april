
#! /usr/bin/python

import os
import sys

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

""" Returns april bin file path """
def get_april_path():
    return os.path.join(get_root_path(), "bin/april")

""" Returns the path that contains test files """
def get_test_path():
    return os.path.join(get_root_path(), "test/unittest")

""" Runs a code file or project file or solution file """
def run_file(file):
    april_path = get_april_path()
    test_file = os.path.join(get_test_path(), file)

    if not os.path.exists(test_file):
        raise RuntimeError("code file " + test_file + " not exists")

    command = april_path + " run " + file;
    print(command)
    #s = os.system(april)
    #return s


##############################################################################

class Base:

    def run(self, file):
        s = run_file(file)
        print(s)
