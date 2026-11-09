#! /usr/bin/python

import pytest
import common


class TestEvent(common.Base):
    """Events: the handlers that are added and removed (+=, -=), the accessors (add, remove,
    raise), the events that are static, abstract, virtual, of an interface, and generic."""

    @pytest.fixture(params=common.code_files('event'))
    def code_file(self, request):
        return request.param

    def test_april(self, code_file):
        self.run('event/' + code_file)
