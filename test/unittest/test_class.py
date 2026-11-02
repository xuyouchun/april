#! /usr/bin/python

import pytest
import common


class TestClass(common.Base):
    """Classes: constructors, static fields, operator overloads, namespaces."""

    @pytest.fixture(params=common.code_files('class'))
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('class/' + code_file)
