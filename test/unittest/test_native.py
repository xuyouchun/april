#! /usr/bin/python

# Tests of the native back end: every test program is built to an executable with
# `april native --build`, run, and its output compared with the EXPECT block, like
# the interpreter tests do.
#
# Programs that use something the back end does not support yet are reported as xfail
# (the message of april is the reason); anything else that goes wrong fails the test:
# a clang error, a crash, or an unexpected output.
#
# Needs clang++ (LLVM 15+) and the static libraries of april: set CLANG and APRIL_LIBS,
# or have them in PATH / <root>/build.

import os
import re
import shutil
import subprocess
import sys

import pytest

import common


def _april_path():
    path = common.get_april_path()
    return path + ".exe" if sys.platform == "win32" else path


def _clang():
    return os.environ.get("CLANG") or shutil.which("clang++")


def _libs_dir():
    candidates = [os.environ.get("APRIL_LIBS"),
                  common.get_absolute_path("build"),
                  common.get_absolute_path("libs")]

    for path in candidates:
        if path and os.path.isfile(os.path.join(path, "libnative.a")):
            return path

    return None


def _read_expect(file):
    matched = False
    lines = []

    with open(file, "r") as f:
        for line in f:
            if re.match(r"\s*/\*\s*EXPECT", line):
                matched = True
                continue

            if matched:
                if re.match(r"\s*\*/", line):
                    break

                lines.append(line)

    assert matched, "no EXPECT block in " + file
    return "".join(lines)


def _lines(text):
    text = re.sub(r"^[\r\n]*|[\r\n]*$", "", text.replace("\r", ""))
    return re.split(r"\n+", text)


def _all_files():
    result = []
    for directory in sorted(os.listdir(common.get_test_path())):
        path = os.path.join(common.get_test_path(), directory)
        # "errors" holds the programs that must be rejected, they have no output.
        if not os.path.isdir(path) or directory.startswith((".", "_")) or directory == "errors":
            continue

        for name in sorted(os.listdir(path)):
            if name.endswith(".cs"):
                result.append(directory + "/" + name)

    return result


@pytest.fixture(scope="module")
def tools():
    april = _april_path()
    clang = _clang()
    libs = _libs_dir()
    system_apl = common.get_absolute_path("System.apl")

    if not os.path.isfile(april):
        pytest.skip("april is not built")

    if not os.path.isfile(system_apl):
        pytest.skip("System.apl is not built")

    if clang is None:
        pytest.skip("clang++ not found (set CLANG)")

    if libs is None:
        pytest.skip("libnative.a not found (set APRIL_LIBS)")

    return april, clang, libs, system_apl


def _build(code_file, tmp_path, tools):
    """Builds a test program in tmp_path, returns the path of the executable.
    A program that uses what the back end does not support yet is reported as xfail."""
    april, clang, libs, system_apl = tools
    source = os.path.join(common.get_test_path(), code_file)

    # Every program is built in its own directory: the assembly is named after the
    # directory of the source, and written to the current directory.
    result = subprocess.run(
        [april, "native", "--build", "--clang", clang, "--libs", libs, source, system_apl],
        cwd=str(tmp_path), stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        universal_newlines=True, errors="replace")

    log = re.sub(r"\x1b\[[0-9;]*m", "", result.stdout)

    if "native: unsupported" in log or "native: inconsistent" in log:
        first = [l for l in log.splitlines() if l.startswith("native:")][-1]
        pytest.xfail(re.sub(r" \[.*\]$", "", first))

    assert result.returncode == 0, "april native failed:\n" + log

    exe = [p for p in tmp_path.iterdir() if p.suffix in (".exe", ".native")]
    assert len(exe) == 1, "no executable was built:\n" + log

    return source, exe[0]


@pytest.mark.parametrize("code_file", _all_files())
def test_native(code_file, tools, tmp_path):
    source, exe = _build(code_file, tmp_path, tools)

    # Run normally, and with a collection at every allocation: a live object that the
    # collector frees changes the output (or crashes).
    for extra in ({}, {"APRIL_GC_STRESS": "1"}):
        env = dict(os.environ)
        env.update(extra)

        run = subprocess.run([str(exe)], cwd=str(tmp_path), stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT, universal_newlines=True,
                             errors="replace", env=env)

        what = " (APRIL_GC_STRESS)" if extra else ""
        assert run.returncode == 0, "the program exited with %d%s:\n%s" % (
            run.returncode, what, run.stdout)
        assert _lines(run.stdout) == _lines(_read_expect(source)), "output differs" + what


def test_native_gc_reclaims_memory(tools, tmp_path):
    """The garbage of a program is freed: the heap stays far smaller than what was allocated."""
    source, exe = _build("native/gc.cs", tmp_path, tools)

    env = dict(os.environ)
    env["APRIL_GC_STATS"] = "1"
    env["APRIL_GC_THRESHOLD"] = "200000"

    run = subprocess.run([str(exe)], cwd=str(tmp_path), stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE, universal_newlines=True,
                         errors="replace", env=env)

    assert run.returncode == 0, run.stderr
    assert _lines(run.stdout) == _lines(_read_expect(source))

    stats = re.search(r"collections=(\d+) peak=(\d+)", run.stderr)
    assert stats, "no statistics: " + run.stderr

    collections, peak = int(stats.group(1)), int(stats.group(2))

    # About 14 MB are allocated, almost all garbage.
    assert collections > 10
    assert peak < 3 * 1000 * 1000
