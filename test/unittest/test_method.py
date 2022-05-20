#! /usr/bin/python

import pytest
import common


class TestMethod(common.Base):

    @pytest.fixture(params=[
        'instance_call.cs',
        'instance_call_ref_out.cs',
        'instance_call_struct.cs'
    ])
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('method/' + code_file)


