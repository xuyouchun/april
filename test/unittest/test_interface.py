#! /usr/bin/python

import pytest
import common

class TestInterface(common.Base):

    @pytest.fixture(params=[
        'simple_interface.cs',
        'multipy_interfaces.cs',
        'generic_interface.cs'
    ])
    def code_file(self, request):
        return request.param

    def test_interface(self, code_file):
        self.run('interface/' + code_file)


