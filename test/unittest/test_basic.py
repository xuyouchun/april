
#! /usr/bin/python

import pytest
import common


class TestBasic(common.Base):

    @pytest.fixture(params=[
        'simple.cs',
        'expressions.cs',
        'statements.cs',
    ])
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('basic/' + code_file)


