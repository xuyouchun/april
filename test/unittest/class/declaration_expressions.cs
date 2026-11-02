// Variables that are declared in an expression: an out argument (out int value, out var value),
// and the pattern of is (o is Pt p). The variable is that of the block that has the statement,
// it is assigned when the pattern matches.

/* EXPECT

7
8
4
6
-1
8
-2
5000000000
3
42
0
43
7
two
10

*/

import System;
using System;
using System.Collections.Generic;
struct P { public int X; public int Y; }
class C { public int N; }
class Program {
  static void Make(out P p, out C c) { p = new P(); p.X = 3; p.Y = 4; c = new C(); c.N = 8; }
  static int Guard(Object o) {
    if (!(o is int i))
      return -1;
    return i + 1;
  }
  static int Guard2(Object o) {
    if (o is not C c)
      return -2;
    return c.N;
  }
  static void GetTwo(out int a, out String b) { a = 7; b = "two"; }
  [EntryPoint] public static void Main() {
    Make(out P p, out C c);
    Console.WriteLine(p.X + p.Y);
    Console.WriteLine(c.N);
    Object bp = p;
    if (bp is P q) Console.WriteLine(q.Y);
    Console.WriteLine(Guard(5));
    Console.WriteLine(Guard("x"));
    Console.WriteLine(Guard2(c));
    Console.WriteLine(Guard2(1));
    Object big = 5000000000;
    if (big is long l) Console.WriteLine(l);
    int total = 0;
    for (int k = 0; k < 3; k++) {
      Object item = k;
      if (item is int v) total += v;
    }
    Console.WriteLine(total);

    Dictionary<String, int> d = new Dictionary<String, int>(new StringComparer());
    d.Add("key", 42);
    if (d.TryGetValue("key", out int value))
        Console.WriteLine(value);
    if (!d.TryGetValue("none", out int missing))
        Console.WriteLine(missing);
    d.TryGetValue("key", out var v2);
    Console.WriteLine(v2 + 1);
    GetTwo(out int x, out String y);
    Console.WriteLine(x);
    Console.WriteLine(y);
    GetTwo(out var x2, out var y2);
    Console.WriteLine(x2 + y2.Length);
  }
}
