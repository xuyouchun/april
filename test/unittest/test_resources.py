#! /usr/bin/python

# Tests of the resources: the files of <resources> in a project are embedded in the assembly, and
# System.Runtime.Resource reads them (the interpreter, the JIT and the executables).

import os
import shutil
import subprocess
import sys

import pytest

import common
from test_cli import _april, native, clang


def _april_run(args, cache, cwd, env_extra=None):
    env = dict(os.environ)
    env["APRIL_CACHE"] = str(cache)
    env["PYTHONIOENCODING"] = "utf-8"
    env.update(env_extra or {})

    return subprocess.run([_april()] + args, cwd=str(cwd), env=env,
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                          universal_newlines=True, encoding="utf-8", errors="replace")


PROJECT = """<?xml version="1.0" encoding="utf-8" ?>

<project>

    <name>App</name>

    <files>
        <file><path>*.cs</path></file>
    </files>

    <resources>
        <resource><path>data/*.bin</path></resource>
        <resource><path>hello.txt</path><name>App.Greeting</name></resource>
        <resource><path>text/*.txt</path></resource>
    </resources>

</project>
"""

PROGRAM = """import System;

using System;
using System.Runtime;
using System.Text;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("exists " + Resource.Exists("App.data.numbers.bin") + " " + Resource.Exists("App.Greeting")
            + " " + Resource.Exists("App.text.a.txt") + " " + Resource.Exists("hello.txt") + " " + Resource.Exists("nothing"));

        UInt8[] numbers = Resource.GetBytes("App.data.numbers.bin");
        Int32 sum = 0;
        for (Int32 k = 0; k < numbers.Length; k++)
            sum += (Int32)numbers[k];

        Console.WriteLine("numbers " + numbers.Length + " " + numbers[0] + " " + numbers[255] + " " + sum);

        String greeting = Encoding.UTF8.GetString(Resource.GetBytes("App.Greeting"));
        Console.WriteLine("greeting " + greeting + " " + greeting.Length);

        Console.WriteLine("empty " + Resource.GetSize("App.data.empty.bin") + " " + Resource.GetBytes("App.data.empty.bin").Length);

        ReadOnlySpan<UInt8> span = Resource.GetSpan("App.data.numbers.bin");
        Console.WriteLine("span " + span.Length + " " + span[7] + " " + span.Slice(250)[5]);

        Console.WriteLine("texts " + Encoding.UTF8.GetString(Resource.GetBytes("App.text.a.txt")) + Encoding.UTF8.GetString(Resource.GetBytes("App.text.b.txt")));

        String caught = "no";
        try { Resource.GetBytes("App.missing"); }
        catch (ArgumentException) { caught = "caught"; }

        String caughtSize = "no";
        try { Resource.GetSize("App.missing"); }
        catch (ArgumentException) { caughtSize = "caught"; }

        Console.WriteLine("missing " + caught + " " + caughtSize);

        // The address of the bytes: no copy.
        Ptr<UInt8> pointer = Resource.GetPtr("App.data.numbers.bin");
        Console.WriteLine("pointer " + pointer[0] + " " + pointer[255] + " " + pointer[100] + " " + Resource.GetPtr("App.missing").IsNull);

        // Two arrays of one resource are two copies.
        UInt8[] first = Resource.GetBytes("App.data.numbers.bin");
        first[0] = 99;
        Console.WriteLine("copy " + Resource.GetBytes("App.data.numbers.bin")[0] + " " + first[0]);
    }
}
"""

EXPECTED = """exists True True True False False
numbers 256 0 255 32640
greeting 你好，世界 5
empty 0 0
span 256 7 255
texts AB
missing caught caught
pointer 0 255 100 True
copy 0 99"""


def _make_project(root):
    (root / "data").mkdir()
    (root / "text").mkdir()

    (root / "App.project").write_text(PROJECT, encoding="utf-8")
    (root / "main.cs").write_text(PROGRAM, encoding="utf-8")
    (root / "data" / "numbers.bin").write_bytes(bytes(range(256)))
    (root / "data" / "empty.bin").write_bytes(b"")
    (root / "hello.txt").write_bytes("你好，世界".encode("utf-8"))
    (root / "text" / "a.txt").write_bytes(b"A")
    (root / "text" / "b.txt").write_bytes(b"B")

    return root / "App.project"


def _check(r):
    assert r.returncode == 0, r.stdout
    assert [l.rstrip() for l in r.stdout.strip().splitlines()] == EXPECTED.splitlines(), r.stdout


def test_resources_interpret(tmp_path):
    project = _make_project(tmp_path)

    _check(_april_run(["run", "--interpret", str(project)], tmp_path / "cache", tmp_path))


@native
def test_resources_jit(tmp_path):
    project = _make_project(tmp_path)

    _check(_april_run(["run", str(project)], tmp_path / "cache", tmp_path))


def test_resource_change_is_built_again(tmp_path):
    project = _make_project(tmp_path)
    cache = tmp_path / "cache"

    _check(_april_run(["run", "--interpret", str(project)], cache, tmp_path))

    # A change of a file of the resources makes a new build.
    (tmp_path / "text" / "b.txt").write_bytes(b"C")
    r = _april_run(["run", "--interpret", str(project)], cache, tmp_path)

    assert r.returncode == 0, r.stdout
    assert "texts AC" in r.stdout, r.stdout


def test_resource_file_is_missing(tmp_path):
    project = _make_project(tmp_path)
    (tmp_path / "hello.txt").unlink()

    r = _april_run(["run", "--interpret", str(project)], tmp_path / "cache", tmp_path)

    assert "exists True False" in r.stdout or r.returncode != 0, r.stdout


@clang
def test_resources_executable(tmp_path):
    project = _make_project(tmp_path)

    exe = tmp_path / ("app.exe" if sys.platform == "win32" else "app")
    r = _april_run(["compile", "--native", "-o", str(exe), str(project)], tmp_path / "cache", tmp_path)
    assert r.returncode == 0, r.stdout

    # The resources are in the executable: the files are not needed.
    shutil.rmtree(tmp_path / "data")
    shutil.rmtree(tmp_path / "text")
    (tmp_path / "hello.txt").unlink()

    out = subprocess.run([str(exe)], stdout=subprocess.PIPE, universal_newlines=True,
                         encoding="utf-8", errors="replace")
    assert out.returncode == 0, out.stdout
    assert [l.rstrip() for l in out.stdout.strip().splitlines()] == EXPECTED.splitlines(), out.stdout
