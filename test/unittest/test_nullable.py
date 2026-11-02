#! /usr/bin/python

import os
import re
import subprocess
import pytest
import common


def _files():
    path = os.path.join(common.get_test_path(), 'nullable')
    return sorted(f for f in os.listdir(path) if f.endswith('.cs'))


class TestNullable:
    """The null states of the references (#nullable enable): the programs compile, the warnings
    are those of the comment: // EXPECT_WARNING <codes in order>, or // EXPECT_WARNING none."""

    @pytest.mark.parametrize('code_file', _files())
    def test_nullable(self, code_file, tmp_path):
        path = os.path.join(common.get_test_path(), 'nullable', code_file)
        text = open(path).read()
        expected = re.search(r'EXPECT_WARNING (.*?)\s*\*?/?\n', text).group(1).split()

        # A program that was built before is not compiled again: it has no warning then.
        env = dict(os.environ, APRIL_CACHE=str(tmp_path))

        command = [common.get_april_path(), 'run', '--interpret', path]
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                universal_newlines=True, errors='replace', env=env)

        output = re.sub(r'\x1b\[[0-9;]*m', '', result.stdout)
        assert result.returncode == 0, 'status %d:\n%s' % (result.returncode, output)

        warnings = re.findall(r'warning (?:CS)?(\d+):', output)
        assert warnings == ([] if expected == ['none'] else expected), output


class TestNullableSwitch:
    """The analysis is enabled for all the files of a compile: by the option --nullable, or by
    <nullable>enable</nullable> in the project. #nullable disable stops it in a file."""

    def _warnings(self, tmp_path, *args):
        env = dict(os.environ, APRIL_CACHE=str(tmp_path))
        directory = os.path.join(common.get_test_path(), 'nullable', 'project')

        command = [common.get_april_path(), 'run', '--interpret'] + list(args)
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                universal_newlines=True, errors='replace', env=env,
                                cwd=directory)

        output = re.sub(r'\x1b\[[0-9;]*m', '', result.stdout)
        assert result.returncode == 0, 'status %d:\n%s' % (result.returncode, output)
        assert 'done' in output, output

        return re.findall(r'warning (?:CS)?(\d+):', output)

    def test_default(self, tmp_path):
        assert self._warnings(tmp_path, 'main.cs') == []

    def test_option(self, tmp_path):
        assert self._warnings(tmp_path, '--nullable', 'main.cs') == ['4005']

    def test_project_enabled(self, tmp_path):
        # other.cs has #nullable disable: only main.cs warns.
        assert self._warnings(tmp_path, 'enabled.project') == ['4005']

    def test_project_plain(self, tmp_path):
        assert self._warnings(tmp_path, 'plain.project') == []
