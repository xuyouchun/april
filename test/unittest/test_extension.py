#! /usr/bin/python

import os
import re
import subprocess
import common


class TestExtension:
    """The extension methods of another assembly: the library is compiled with the program (a
    solution), the program finds the methods by the marks of the assembly and of its classes."""

    def test_assembly(self, tmp_path):
        directory = os.path.join(common.get_test_path(), 'class', 'extension')
        april = common.get_april_path()

        def run(*args):
            result = subprocess.run((april,) + args, stdout=subprocess.PIPE,
                                    stderr=subprocess.STDOUT, universal_newlines=True,
                                    errors='replace', cwd=directory)

            return result.returncode, re.sub(r'\x1b\[[0-9;]*m', '', result.stdout)

        code, output = run('compile', 'extension.solution', '-o', str(tmp_path))
        assert code == 0, output

        code, output = run('run', '--interpret', os.path.join(str(tmp_path), 'app.apl'))
        assert code == 0, output
        assert output.split() == ['hi!', '8', '12'], output

        # The library has the attribute that marks its extension methods.
        with open(os.path.join(str(tmp_path), 'extlib.apl'), 'rb') as f:
            data = f.read()

        assert b'ExtensionAttribute' in data or 'ExtensionAttribute'.encode('utf-16-le') in data
