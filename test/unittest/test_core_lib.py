#! /usr/bin/python

import pytest
import common


class TestCoreLib(common.Base):
    """The library (core_lib): the tests are in core_lib/<namespace>/, one folder for each namespace."""

    @pytest.fixture(params=common.code_files_recursive('core_lib'))
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('core_lib/' + code_file)
