#! /usr/bin/python

import os
import re
import subprocess
import pytest
import common


def _files():
    path = os.path.join(common.get_test_path(), 'errors')
    return sorted(f for f in os.listdir(path) if f.endswith('.cs'))


class TestErrors:
    """Programs that are wrong are rejected with a diagnostic: the compiler neither crashes
    nor accepts them. The first comment of a file tells the error: EXPECT_ERROR <regex>."""

    @pytest.mark.parametrize('code_file', _files())
    def test_error(self, code_file):
        path = os.path.join(common.get_test_path(), 'errors', code_file)
        pattern = re.search(r'EXPECT_ERROR (.*?) \*/', open(path).read()).group(1)

        command = [common.get_april_path(), 'run', path, common.get_absolute_path('System.apl')]
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                universal_newlines=True, errors='replace')

        output = re.sub(r'\x1b\[[0-9;]*m', '', result.stdout)
        assert result.returncode == 1, 'status %d:\n%s' % (result.returncode, output)
        assert re.search(pattern, output), output
