#! /usr/bin/python

import os
import re
import subprocess

import pytest
import common


class TestTypedef(common.Base):
    """Aliases of types (using Name<params> = Type;): of types, of generic types, of nested types, with access."""

    @pytest.fixture(params=common.code_files('typedef'))
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('typedef/' + code_file)


class TestTypedefAssembly:
    """The aliases of a library: the public ones are seen by a program that uses it (with their params, their constraints and
    their packs); the internal ones, the ones of a file and the private ones of a type are not."""

    @staticmethod
    def _run(*args):
        directory = os.path.join(common.get_test_path(), 'typedef', 'assembly')
        result = subprocess.run((common.get_april_path(),) + args, stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, universal_newlines=True,
                                errors='replace', cwd=directory)

        return result.returncode, re.sub(r'\x1b\[[0-9;]*m', '', result.stdout)

    def test_public_aliases(self, tmp_path):
        code, output = self._run('compile', 'good.solution', '-o', str(tmp_path))
        assert code == 0, output

        code, output = self._run('run', '--interpret', os.path.join(str(tmp_path), 'app.apl'))
        assert code == 0, output
        assert output.split() == ['5', '6', '7x', '3', '7', '8'], output

    def test_other_aliases(self, tmp_path):
        code, output = self._run('compile', 'bad.solution', '-o', str(tmp_path))
        assert code != 0, output

        for name in ('Hidden', 'FileOnly', 'Holder.Secret'):
            assert 'type "%s" is not defined' % name in output, output

        # The constraints of a public alias are those of the type that it names.
        assert 'must be a non-nullable value type' in output, output
