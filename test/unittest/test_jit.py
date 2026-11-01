#! /usr/bin/python

# Tests of the JIT back end: every test program is run with `april run`, that runs it in the
# process of april with LLVM (april_jit), and its output is compared with the EXPECT block,
# like the interpreter tests do. A program that uses what the native back end does not
# support yet is run with the interpreter by `april run`: it is reported as xfail (--verbose
# tells it), anything else that goes wrong fails the test.
#
# Needs the library april_jit (built when LLVM is found) and the library of LLVM it loads.

import os
import re
import subprocess
import sys

import pytest

import common
from test_native import _all_files, _read_expect, _lines


def _april():
    path = common.get_april_path()
    return path + ".exe" if sys.platform == "win32" else path


def _run(args, cache, env_extra=None):
    env = dict(os.environ)
    env["APRIL_CACHE"] = str(cache)
    env.update(env_extra or {})

    return subprocess.run([_april(), "run", "--verbose"] + args, stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE, universal_newlines=True, errors="replace",
                          env=env)


@pytest.fixture(scope="module")
def jit(tmp_path_factory):
    cache = tmp_path_factory.mktemp("jit-cache")
    probe = os.path.join(common.get_test_path(), "basic", "simple.cs")

    result = _run([probe], cache)
    if "no JIT" in result.stderr or "cannot" in result.stderr and "program" in result.stderr:
        pytest.skip("the JIT is not available: " + result.stderr.strip())

    return cache


@pytest.mark.parametrize("code_file", _all_files())
def test_jit(code_file, jit):
    source = os.path.join(common.get_test_path(), code_file)

    for extra in ({}, {"APRIL_GC_STRESS": "1"}):
        result = _run([source], jit, extra)

        if "the program is run with the interpreter" in result.stderr:
            first = [l for l in result.stderr.splitlines() if l.startswith("native:")][-1]
            pytest.xfail(re.sub(r" \[.*\]$", "", first))

        what = " (APRIL_GC_STRESS)" if extra else ""
        assert result.returncode == 0, "exit code %d%s:\n%s\n%s" % (
            result.returncode, what, result.stdout, result.stderr)
        assert _lines(result.stdout) == _lines(_read_expect(source)), "output differs" + what
