#! /usr/bin/python

import pytest
import common


class TestLq(common.Base):
    """The lq language: the query expressions that are written in the code of another language
    (``lq from x in xs select x, <%lq from x in xs select x %>), analyzed by the patterns of lq and
    made to the calls of System.Linq."""

    @pytest.fixture(params=common.code_files('lq'))
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('lq/' + code_file)
