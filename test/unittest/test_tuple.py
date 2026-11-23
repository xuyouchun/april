#! /usr/bin/python

import os
import re
import subprocess

import common


class TestTupleAssembly:
    """The names of the items of a tuple type (Int32 Number, String Str) are in the metadata of a library: a program that uses it
    reads the items by the names (of a property, a method, a param, a typedef)."""

    @staticmethod
    def _run(*args):
        directory = os.path.join(common.get_test_path(), 'tuple_assembly')
        result = subprocess.run((common.get_april_path(),) + args, stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, universal_newlines=True,
                                errors='replace', cwd=directory)

        return result.returncode, re.sub(r'\x1b\[[0-9;]*m', '', result.stdout)

    def test_names_of_a_library(self, tmp_path):
        code, output = self._run('compile', 'tuple.solution', '-o', str(tmp_path))
        assert code == 0, output

        code, output = self._run('run', '--interpret', os.path.join(str(tmp_path), 'app.apl'))
        assert code == 0, output
        assert output.split('\n')[:-1] == ['3x4', 'square 4', 'item7', '30', '8,9', 'ba', 'two2', 'one', 'two', '9', 'five5', '7'], output
