#! /usr/bin/python

import os
import re
import subprocess
import pytest
import common


class TestPartial:
    """The parts of a partial class are in several files of a project, in any order."""

    @pytest.mark.parametrize('project, expected', [
        ('ab.project',      ['27', 'id', '7', 'circle', '3', '2', 'done']),
        ('ba.project',      ['27', 'id', '7', 'circle', '3', '2', 'done']),

        # The initial values of the fields use the usings of their own file.
        ('init_ab.project', ['16']),
        ('init_ba.project', ['16']),
    ])
    def test_files(self, project, expected, tmp_path):
        env = dict(os.environ, APRIL_CACHE=str(tmp_path))
        directory = os.path.join(common.get_test_path(), 'partial', 'project')

        command = [common.get_april_path(), 'run', '--interpret', project]
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                universal_newlines=True, errors='replace', env=env,
                                cwd=directory)

        output = re.sub(r'\x1b\[[0-9;]*m', '', result.stdout)
        assert result.returncode == 0, 'status %d:\n%s' % (result.returncode, output)
        assert output.split() == expected, output
