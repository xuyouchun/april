#! /usr/bin/python

import pytest
import common


class TestBasic(common.Base):
    """Basic language: operators, conversions, literals, strings, arrays, statements."""

    @pytest.fixture(params=common.code_files('basic'))
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('basic/' + code_file)
