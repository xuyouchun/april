#! /usr/bin/python

import pytest
import common


class TestBox(common.Base):

    @pytest.fixture(params=['0001.cs'])
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('box/' + code_file)


