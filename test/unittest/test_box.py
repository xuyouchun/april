#! /usr/bin/python

import pytest
import common


class TestBox(common.Base):

    def test_demo(self):
        self.run("box/abcd.cs")


