#! /usr/bin/python

import pytest
import common

class TestInterface(common.Base):

    @pytest.fixture(params=[
        'simple_interface.cs',
        'multipy_interfaces.cs',
        'generic_interface.cs',
        'generic_math.cs',
        'inherited_static_members.cs',
        'generic_receiver.cs',
        'generic_math_convert.cs',
        'static_generic_methods.cs',
        'operator_checked.cs',
        'generic_math_checked.cs',
        'generic_tuple_items.cs',
        'struct_operator_results.cs',
        'generic_conversions.cs'
    ])
    def code_file(self, request):
        return request.param

    def test_interface(self, code_file):
        self.run('interface/' + code_file)


