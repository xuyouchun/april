// yield return and yield break in methods that return IEnumerable<T> or IEnumerator<T>.
// The items are made one by one, infinite sequences and the order of the effects work.

/* EXPECT

1
2
3
0
2
4
6
a
b
7
8
9
50
60
70
80
make 0
got 0
make 1
got 1
make 2
got 2
0
1
1
2
3
5
8
13
*/
import System;
using System;
using System.Collections.Generic;

class Seq {
  int step = 2;
  public static IEnumerable<Int32> Range(Int32 lo, Int32 hi) {
    for (Int32 i = lo; i < hi; i++) yield return i;
  }
  public IEnumerable<Int32> Evens(Int32 limit) {
    Int32 n = 0;
    while (n < limit) { yield return n; n += step; }
  }
  public static IEnumerable<String> Words() {
    yield return "a";
    yield return "b";
    yield break;
  }
  public static IEnumerator<Int32> Enum3() {
    yield return 7; yield return 8; yield return 9;
  }
  public static IEnumerable<Int32> Filter(IEnumerable<Int32> src, Int32 min) {
    foreach (Int32 v in src) { if (v < min) continue; if (v > 8) yield break; yield return v * 10; }
  }
  public static IEnumerable<Int32> Naturals() {
    Int32 n = 0;
    while (true) { Console.WriteLine("make " + n); yield return n; n++; }
  }
  public static IEnumerable<Int32> Fib() {
    Int32 a = 0; Int32 b = 1;
    while (true) { yield return a; Int32 t = a + b; a = b; b = t; }
  }
}

class Program {
  [EntryPoint] public static void Main() {
    foreach (Int32 v in Seq.Range(1, 4)) Console.WriteLine(v);
    Seq s = new Seq();
    foreach (Int32 v in s.Evens(7)) Console.WriteLine(v);
    foreach (String w in Seq.Words()) Console.WriteLine(w);
    IEnumerator<Int32> e = Seq.Enum3();
    while (e.MoveNext()) Console.WriteLine(e.Current);
    foreach (Int32 v in Seq.Filter(Seq.Range(0, 20), 5)) Console.WriteLine(v);
    foreach (Int32 v in Seq.Naturals()) { Console.WriteLine("got " + v); if (v == 2) break; }
    Int32 count = 0;
    foreach (Int32 v in Seq.Fib()) { Console.WriteLine(v); count++; if (count == 8) break; }
  }
}
