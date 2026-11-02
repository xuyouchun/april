#! /usr/bin/python

import pytest
import common


class TestCustomStruct(common.Base):
    """Custom structs: fields, locals, arguments, returns."""

    @pytest.fixture(params=common.code_files('custom_struct'))
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('custom_struct/' + code_file)
