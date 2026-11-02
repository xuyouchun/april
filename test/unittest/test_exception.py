#! /usr/bin/python

import pytest
import common


class TestException(common.Base):
    """Exceptions: try/catch/finally, runtime checks, using (IDisposable)."""

    @pytest.fixture(params=common.code_files('exception'))
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('exception/' + code_file)
