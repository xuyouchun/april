#! /usr/bin/python

import os
import subprocess
import sys

import common


def test_catalog():
    """docs/diagnostics.md is generated from the sources, and the codes and the messages follow
    the rules of the catalog: a code is used once, a message begins in lower case, has no period,
    names are in double quotes and nothing is misspelled."""
    root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    script = os.path.join(root, 'tools', 'gen_diagnostics.py')

    result = subprocess.run([sys.executable, script, '--check'], stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, universal_newlines=True)

    assert result.returncode == 0, result.stdout
