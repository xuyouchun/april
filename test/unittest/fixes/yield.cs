// yield return and yield break in methods that return IEnumerable<T> or IEnumerator<T>.
// The items are collected when the method is called, they are not made one by one.

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
  }
}
