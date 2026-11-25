#! /usr/bin/python
"""Counts the xils that the compiler writes for the test programs, and checks their structure.

    python tools/xil_corpus.py                 # without and with the passes over the xils: the counts and the difference
    python tools/xil_corpus.py --verify        # the structure of the xils is checked too (APRIL_VERIFY_XIL), a failure is told

The programs of test/unittest (not errors/) are compiled, not run: it takes a few minutes. The semantics of the optimized xils are tested by
the test suites run with the optimization on (see docs/xil.md).
"""

import argparse
import concurrent.futures
import glob
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def april():
    path = os.path.join(ROOT, "bin", "april")
    return path + ".exe" if sys.platform == "win32" else path


def compile_one(args):
    source, opt, verify = args
    env = dict(os.environ)
    env["APRIL_XIL_STATS"] = "1"
    if opt:
        env["APRIL_XIL_OPT"] = "1"
    if verify:
        env["APRIL_VERIFY_XIL"] = "1"

    with tempfile.TemporaryDirectory() as out:
        result = subprocess.run([april(), "compile", source, "-o", out], stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, universal_newlines=True, errors="replace", env=env)

    match = re.search(r"==\s*xil stats:\s*(\d+) methods, (\d+) xils", result.stderr)
    failed = result.returncode != 0 and "xil verify" in (result.stderr + result.stdout)
    return source, (int(match.group(2)) if match else None), failed, result.stderr + result.stdout


def total(files, opt, verify):
    count, missing, failures = 0, 0, []

    with concurrent.futures.ThreadPoolExecutor(max_workers=8) as pool:
        for source, xils, failed, output in pool.map(compile_one, [(f, opt, verify) for f in files]):
            if failed:
                failures.append((source, [l for l in output.splitlines() if "xil verify" in l][:1]))
            if xils is None:
                missing += 1
            else:
                count += xils

    return count, missing, failures


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--verify", action="store_true")
    options = parser.parse_args()

    base = os.path.join(ROOT, "test", "unittest")
    files = sorted(f for f in glob.glob(os.path.join(base, "**", "*.cs"), recursive=True)
                   if os.sep + "errors" + os.sep not in f)

    results = {}
    for opt in (False, True):
        count, missing, failures = total(files, opt, options.verify)
        results[opt] = count
        print("%-12s %d xils in %d programs (%d did not compile)" % ("with passes:" if opt else "without:", count, len(files) - missing, missing))
        for source, lines in failures:
            print("  VERIFY FAILED", os.path.relpath(source, base), lines)

    if len(results) >= 2:
        first, last = results[False], results[True]
        print("change: %+d xils (%+.2f%%)" % (last - first, (last / first - 1) * 100 if first else 0))


if __name__ == "__main__":
    main()
