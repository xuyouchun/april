#! /usr/bin/python

import pytest
import common


class TestMethod(common.Base):

    @pytest.fixture(params=[
        'instance_call.cs',
        'ref_out_arguments.cs',
        'struct_arguments.cs',
        "this_arguments.cs",
        'delegate.cs',
    ])
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('method/' + code_file)


