#! /usr/bin/python
"""Measures the programs of bench/ with the interpreter, and counts their xils.

    python tools/bench.py                     # the times (the median of 5 runs) and the count of the xils
    python tools/bench.py loop fib            # the programs that have these texts in their names
    python tools/bench.py --runs 9            # more runs
    python tools/bench.py --save base.json    # writes the numbers
    python tools/bench.py --compare base.json # the numbers that were saved, and the change of each

    python tools/bench.py --xil-opt           # with the passes over the xils (APRIL_XIL_OPT=1), the default is without them

The time is the one of the run of the program (the compile is done once before, april caches it). The xils are the ones that the compiler
writes for the program (APRIL_XIL_STATS), the library is not counted: it was compiled before.
"""

import argparse
import glob
import json
import os
import re
import statistics
import subprocess
import sys
import tempfile
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def april():
    path = os.path.join(ROOT, "bin", "april")
    return path + ".exe" if sys.platform == "win32" else path


def run(args, env_extra=None, timeout=600):
    env = dict(os.environ)
    env.update(env_extra or {})
    start = time.perf_counter()
    result = subprocess.run([april()] + args, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            universal_newlines=True, errors="replace", env=env, timeout=timeout)
    return time.perf_counter() - start, result


def count_xils(source, env):
    """The count of the xils of the program: the compile is made with APRIL_XIL_STATS."""
    with tempfile.TemporaryDirectory() as out:
        _, result = run(["compile", source, "-o", out], dict(env, APRIL_XIL_STATS="1"))

    match = re.search(r"==\s*xil stats:\s*(\d+) methods, (\d+) xils", result.stderr)
    return int(match.group(2)) if match else None


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("names", nargs="*", help="texts of the names of the programs")
    parser.add_argument("--runs", type=int, default=5)
    parser.add_argument("--xil-opt", action="store_true", help="the passes over the xils are on (APRIL_XIL_OPT=1)")
    parser.add_argument("--save", help="writes the numbers to a file")
    parser.add_argument("--compare", help="reads the numbers of a file, and tells the change")
    options = parser.parse_args()

    files = sorted(glob.glob(os.path.join(ROOT, "bench", "*.cs")))
    if options.names:
        files = [f for f in files if any(n in os.path.basename(f) for n in options.names)]

    baseline = {}
    if options.compare:
        with open(options.compare) as stream:
            baseline = json.load(stream)

    numbers = {}
    env = {"APRIL_XIL_OPT": "1"} if options.xil_opt else {}
    print("%-20s %10s %8s  %s" % ("program", "ms", "xils", "output") + ("   change" if baseline else ""))

    for source in files:
        name = os.path.basename(source)
        flags = ["run", "--interpret", source]

        # The first run compiles the program, the others are measured.
        _, first = run(flags, env)
        if first.returncode != 0:
            print("%-20s FAILED\n%s" % (name, first.stderr[-600:] or first.stdout[-600:]))
            continue

        times = []
        for _ in range(options.runs):
            elapsed, result = run(flags, env)
            times.append(elapsed * 1000)

        xils = count_xils(source, env)
        median = statistics.median(times)
        numbers[name] = {"ms": median, "xils": xils, "output": first.stdout.strip()}

        change = ""
        if name in baseline:
            old = baseline[name]
            change = "   %+.1f%% time" % ((median / old["ms"] - 1) * 100) if old.get("ms") else ""
            if old.get("xils") and xils:
                change += ", %+.1f%% xils" % ((xils / old["xils"] - 1) * 100)

            if old.get("output") != first.stdout.strip():
                change += "   OUTPUT CHANGED"

        print("%-20s %10.0f %8s  %s%s" % (name, median, xils if xils is not None else "-",
                                          first.stdout.strip()[:30], change))

    if options.save:
        with open(options.save, "w") as stream:
            json.dump(numbers, stream, indent=2, sort_keys=True)

        print("written", options.save)


if __name__ == "__main__":
    main()
