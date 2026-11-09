#! /usr/bin/python

import pytest
import common


class TestMath(common.Base):
    """Mathematical calculations that are written in the language: the programs of the directory
    math compute numbers (pi, ...) with the arithmetic of the language only, and their output is
    compared with the digits that are known."""

    @pytest.fixture(params=common.code_files('math'))
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('math/' + code_file)
