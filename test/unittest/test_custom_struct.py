
#! /usr/bin/python

import pytest
import common


class TestCustomStruct(common.Base):

    @pytest.fixture(params=[
        'new_expression.cs',
        'return.cs',
        'delegate.cs',
        'local.cs',
        'field.cs',
        'param.cs',
        'return_variable.cs',
        'pass_argument.cs',
        'operators.cs',
    ])
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('custom_struct/' + code_file)


