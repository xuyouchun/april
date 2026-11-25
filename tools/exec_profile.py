#! /usr/bin/python
"""Runs programs with the profile of the commands of the interpreter (APRIL_EXEC_PROFILE=1) and sums what it tells.

    python tools/exec_profile.py bench     # the programs of bench/: the commands that they execute, and what the superinstructions would save
    python tools/exec_profile.py tests     # the test programs: the methods by calls, the rule of the tiers, the simulated saving

See docs/xil.md (Profile of the commands).
"""

import concurrent.futures, glob, os, re, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
APRIL = os.path.join(ROOT, "bin", "april.exe" if sys.platform == "win32" else "april")


def run(f):
    env = dict(os.environ, APRIL_EXEC_PROFILE="1")
    try:
        r = subprocess.run([APRIL, "run", "--interpret", f], capture_output=True, text=True, errors="replace",
                           env=env, timeout=300, cwd=os.path.join(ROOT, "test", "unittest"))
    except subprocess.TimeoutExpired:
        return f, None
    return f, r.stderr


def parse(err):
    d = {}
    m = re.search(r"methods parsed: (\d+), entered: (\d+), xils: (\d+), commands executed: (\d+)", err)
    if not m:
        return None
    d["parsed"], d["entered"], d["xils"], d["executed"] = map(int, m.groups())
    m = re.search(r"time to parse the commands: (\d+) us, of that the cut points: (\d+) us", err)
    d["parse_us"], d["cut_us"] = map(int, m.groups())
    d["thr"] = {}
    for m in re.finditer(r"  >= (\d+):\t(\d+)\t([\d.]+)%\t([\d.]+)%", err):
        d["thr"][int(m.group(1))] = (int(m.group(2)), float(m.group(4)))
    d["rule"] = {}
    for m in re.finditer(r"  rule \(loop, or calls >= (\d+)\):	(\d+)	([\d.]+)%	([\d.]+)%", err):
        d["rule"][int(m.group(1))] = (int(m.group(2)), float(m.group(4)))
    m = re.search(r"with a loop:\t(\d+)\t[\d.]+%\t([\d.]+)%  \(of which entered < 20 times: ([\d.]+)%\)", err)
    d["loop"] = (int(m.group(1)), float(m.group(2)), float(m.group(3))) if m else (0, 0, 0)
    m = re.search(r"after the superinstructions: (\d+)  \(saved ([\d.]+)%, ([\d.]+)% of it in methods that have a loop\)", err)
    d["saved"] = float(m.group(2)) if m else 0
    m = re.search(r"if the places of jumps and blocks did not cut: saved ([\d.]+)%", err)
    d["saved_nocut"] = float(m.group(1)) if m else 0
    d["patterns"] = re.findall(r"^  (\d+)\t(\d+)\t([a-z_ ]+)$", err.split("-- simulation")[-1], re.M)
    return d


if __name__ == "__main__":
    mode = sys.argv[1]
    if mode == "bench":
        files = sorted(glob.glob(os.path.join(ROOT, "bench", "*.cs")))
    else:
        files = sorted(f for f in glob.glob(os.path.join(ROOT, "test", "unittest", "**", "*.cs"), recursive=True)
                       if os.sep + "errors" + os.sep not in f)

    results = {}
    with concurrent.futures.ThreadPoolExecutor(4) as ex:
        for f, err in ex.map(run, files):
            if err is None:
                continue
            d = parse(err)
            if d:
                results[f] = d

    if mode == "bench":
        print("%-18s %12s %7s %7s %6s %9s %9s" % ("program", "executed", "saved", "nocut", "loop%", "parse_us", "cut_us"))
        for f, d in results.items():
            print("%-18s %12d %6.1f%% %6.1f%% %5.1f%% %9d %9d" % (os.path.basename(f), d["executed"], d["saved"], d["saved_nocut"],
                                                                d["loop"][1], d["parse_us"], d["cut_us"]))
        agg = {}
        for f, d in results.items():
            for count, saved, name in d["patterns"]:
                a = agg.setdefault(name.strip(), [0, 0])
                a[0] += int(count)
                a[1] += int(saved)
        total = sum(d["executed"] for d in results.values())
        print("\nall the benchmarks: executed %d" % total)
        for name, (uses, saved) in sorted(agg.items(), key=lambda kv: -kv[1][1])[:30]:
            print("  %12d uses %12d saved (%4.1f%%)  %s" % (uses, saved, saved * 100.0 / total, name))
    else:
        n = len(results)
        print("test programs: %d" % n)
        executed = sum(d["executed"] for d in results.values())
        entered = sum(d["entered"] for d in results.values())
        parse_us = sum(d["parse_us"] for d in results.values())
        cut_us = sum(d["cut_us"] for d in results.values())
        print("methods entered (all programs): %d, commands executed: %d, parse time %d us (cut points %d us)" % (entered, executed, parse_us, cut_us))
        for t in (1, 2, 5, 10, 20, 50, 100, 500, 1000):
            methods = sum(d["thr"].get(t, (0, 0))[0] for d in results.values())
            share = sum(d["thr"].get(t, (0, 0))[1] * d["executed"] / 100.0 for d in results.values())
            print("  calls >= %-5d methods %6d (%5.1f%%)   commands %5.1f%%" % (t, methods, methods * 100.0 / entered, share * 100.0 / executed))
        loops = sum(d["loop"][0] for d in results.values())
        lshare = sum(d["loop"][1] * d["executed"] / 100.0 for d in results.values())
        lcold = sum(d["loop"][2] * d["executed"] / 100.0 for d in results.values())
        print("  with a loop: methods %d (%5.1f%%), commands %5.1f%%, of that in methods entered < 20 times %5.1f%%" %
              (loops, loops * 100.0 / entered, lshare * 100.0 / executed, lcold * 100.0 / executed))
        for t in (10, 20, 50, 100):
            methods = sum(d["rule"].get(t, (0, 0))[0] for d in results.values())
            share = sum(d["rule"].get(t, (0, 0))[1] * d["executed"] / 100.0 for d in results.values())
            print("  rule (loop or calls >= %-3d): methods %6d (%5.1f%%)   commands %5.1f%%" % (t, methods, methods * 100.0 / entered, share * 100.0 / executed))
        saved = sum(d["saved"] * d["executed"] / 100.0 for d in results.values())
        nocut = sum(d["saved_nocut"] * d["executed"] / 100.0 for d in results.values())
        print("  simulated saving: %5.1f%% of the commands (without the cuts: %5.1f%%)" % (saved * 100.0 / executed, nocut * 100.0 / executed))
