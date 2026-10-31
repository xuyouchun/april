#! /usr/bin/python

import os
import pytest
import common


def _files():
    path = os.path.join(common.get_test_path(), 'fixes')
    return sorted(f for f in os.listdir(path) if f.endswith('.cs'))


class TestFixes(common.Base):
    """Regression tests of bugs of the compiler and the interpreter that were fixed."""

    @pytest.fixture(params=_files())
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('fixes/' + code_file)
