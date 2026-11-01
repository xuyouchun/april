#! /usr/bin/python

import pytest
import common


class TestCollection(common.Base):
    """Collections: lists, dictionaries, the collection interfaces, iterators (yield)."""

    @pytest.fixture(params=common.code_files('collection'))
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('collection/' + code_file)
