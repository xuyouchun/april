#! /usr/bin/python

# Tests of the command line: `april run` builds the program native in the directory of the
# builds (APRIL_CACHE) and builds it again only when something has changed; System.apl is
# not given; `april compile --native` makes an executable; `april native` is gone.

import os
import shutil
import subprocess
import sys

import pytest

import common


def _april():
    path = common.get_april_path()
    return path + ".exe" if sys.platform == "win32" else path


def _libs_dir():
    candidates = [os.environ.get("APRIL_LIBS"),
                  common.get_absolute_path("build"),
                  common.get_absolute_path("libs")]

    for path in candidates:
        if path and os.path.isfile(os.path.join(path, "libnative.a")):
            return path

    return None


def _native_available():
    clang = os.environ.get("CLANG") or shutil.which("clang++")
    return clang is not None and _libs_dir() is not None


def _jit_available():
    name = "april_jit.dll" if sys.platform == "win32" else "libapril_jit.so"
    return os.path.isfile(os.path.join(os.path.dirname(_april()), name))


# `april run` is native with the JIT (LLVM), or with clang; the executables need clang.
native = pytest.mark.skipif(not (_native_available() or _jit_available()),
                            reason="neither the JIT (april_jit) nor clang++ and libnative.a")
clang = pytest.mark.skipif(not _native_available(), reason="clang++ or libnative.a not found")

PROGRAM = """import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(%d);
    }
}
"""


def _april_run(args, cache, cwd, env_extra=None):
    env = dict(os.environ)
    env["APRIL_CACHE"] = str(cache)
    env.update(env_extra or {})

    return subprocess.run([_april()] + args, cwd=str(cwd), env=env,
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                          universal_newlines=True, errors="replace")


def _entries(cache):
    return sorted(p.name for p in cache.iterdir() if p.is_dir()) if cache.exists() else []


def _write(path, value):
    path.write_text(PROGRAM % value)


@native
def test_run_is_native_and_cached(tmp_path):
    source = tmp_path / "main.cs"
    cache = tmp_path / "cache"
    _write(source, 41)

    r = _april_run(["run", str(source)], cache, tmp_path)
    assert r.returncode == 0 and r.stdout.strip() == "41", r.stdout

    entries = _entries(cache)
    assert len(entries) == 1

    # The program is in the build (the object file of the JIT, or the executable): a second run
    # uses it.
    entry = cache / entries[0]
    if (entry / "program.o").is_file():
        exe = str(entry / "program.o")
    else:
        exe = (entry / "native.txt").read_text().strip()

    assert os.path.isfile(exe)
    stamp = os.path.getmtime(exe)

    r = _april_run(["run", str(source)], cache, tmp_path)
    assert r.returncode == 0 and r.stdout.strip() == "41", r.stdout
    assert _entries(cache) == entries
    assert os.path.getmtime(exe) == stamp

    # A change of the source makes a new build.
    _write(source, 42)
    r = _april_run(["run", str(source)], cache, tmp_path)
    assert r.returncode == 0 and r.stdout.strip() == "42", r.stdout
    assert len(_entries(cache)) == 2


def test_run_interpret(tmp_path):
    source = tmp_path / "main.cs"
    cache = tmp_path / "cache"
    _write(source, 7)

    r = _april_run(["run", "--interpret", str(source)], cache, tmp_path)
    assert r.returncode == 0 and r.stdout.strip() == "7", r.stdout

    # Only the assemblies were built.
    entry = cache / _entries(cache)[0]
    assert not (entry / "native.txt").exists()
    assert not (entry / "program.o").exists()

    # The assemblies are not written to the current directory.
    assert not list(tmp_path.glob("*.apl"))


def test_system_assembly_is_not_given(tmp_path):
    source = tmp_path / "main.cs"
    _write(source, 1)

    system_apl = common.get_absolute_path("System.apl")
    r = _april_run(["run", "--interpret", str(source), system_apl], tmp_path / "cache", tmp_path)

    assert r.returncode == 1
    assert "cannot be specified" in r.stdout


def test_exit_code(tmp_path):
    source = tmp_path / "main.cs"
    cache = tmp_path / "cache"
    source.write_text(PROGRAM.replace("Console.WriteLine(%d);", "throw new OverflowException();")
                      % ())

    r = _april_run(["run", "--interpret", str(source)], cache, tmp_path)
    assert r.returncode != 0


def test_native_command_is_gone(tmp_path):
    r = _april_run(["native"], tmp_path / "cache", tmp_path)

    assert r.returncode == 1
    assert "unknown command" in r.stdout


@clang
def test_compile_native(tmp_path):
    source = tmp_path / "main.cs"
    cache = tmp_path / "cache"
    _write(source, 5)

    exe = tmp_path / ("prog.exe" if sys.platform == "win32" else "prog")
    r = _april_run(["compile", "--native", "-o", str(exe), str(source)], cache, tmp_path)
    assert r.returncode == 0, r.stdout
    assert exe.is_file()

    out = subprocess.run([str(exe)], stdout=subprocess.PIPE, universal_newlines=True)
    assert out.stdout.strip() == "5"

    # Without -o: the executable is written to the current directory, named after the assembly.
    r = _april_run(["compile", "--native", str(source)], cache, tmp_path)
    assert r.returncode == 0, r.stdout
    assert [p for p in tmp_path.iterdir() if p.stem == tmp_path.name]


def test_compile_makes_assemblies(tmp_path):
    source = tmp_path / "main.cs"
    out = tmp_path / "out"
    out.mkdir()
    _write(source, 1)

    r = _april_run(["compile", "-o", str(out), str(source)], tmp_path / "cache", tmp_path)
    assert r.returncode == 0, r.stdout
    assert list(out.glob("*.apl"))
