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
    nor accepts them. The first comment of a file tells the error: EXPECT_ERROR <regex>, and
    optionally the place of the first error (EXPECT_AT <line>:<column>) and what is not told
    (EXPECT_NOT <regex>), and its code (EXPECT_CODE AP0001)."""

    @pytest.mark.parametrize('code_file', _files())
    def test_error(self, code_file):
        path = os.path.join(common.get_test_path(), 'errors', code_file)
        pattern = re.search(r'EXPECT_ERROR (.*?) \*/', open(path).read()).group(1)

        command = [common.get_april_path(), 'run', '--interpret', path]
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                universal_newlines=True, errors='replace')

        output = re.sub(r'\x1b\[[0-9;]*m', '', result.stdout)
        assert result.returncode == 1, 'status %d:\n%s' % (result.returncode, output)
        assert re.search(pattern, output), output

        # Where the first error is: EXPECT_AT <line>:<column>.
        text = open(path).read()
        position = re.search(r'EXPECT_AT (\d+):(\d+)', text)
        if position is not None:
            first = re.search(r'error[^\n]*\n\s*\S+:(\d+):(\d+)', output)
            assert first is not None and first.groups() == position.groups(), output

        # The code of the first error: EXPECT_CODE AP0011.
        code = re.search(r'EXPECT_CODE (AP\d{4})', text)
        if code is not None:
            first_code = re.search(r'error (AP\d{4}):', output)
            assert first_code is not None and first_code.group(1) == code.group(1), output

        # Messages that must not be there: EXPECT_NOT <regex>, such as the errors that follow a
        # syntax error.
        for unexpected in re.findall(r'EXPECT_NOT (.*?) \*/', text):
            assert not re.search(unexpected, output), output
