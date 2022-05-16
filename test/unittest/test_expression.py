
#! /usr/bin/python

import pytest
import common

class TestExpression(common.Base):

    @pytest.fixture(params=[
        'simple_expression.cs',
    ])
    def code_file(self, request):
        return request.param

    def test_expression(self, code_file):
        self.run('expression/' + code_file)


