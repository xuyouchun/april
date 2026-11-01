#! /usr/bin/python

import pytest
import common


class TestMethod(common.Base):
    """Methods: calls, arguments, default params, delegates, generic return types."""

    @pytest.fixture(params=common.code_files('method'))
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('method/' + code_file)
