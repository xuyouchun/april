#! /usr/bin/python

import pytest
import common


class TestLambda(common.Base):
    """Lambdas: typed and implicit params, instance methods, captured variables."""

    @pytest.fixture(params=common.code_files('lambda'))
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('lambda/' + code_file)
