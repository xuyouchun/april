// IDisposable, the using statement, and foreach disposing its enumerator.

/* EXPECT

open a
in a
close a
open b
open c
in bc
close c
close b
open early
close early
5
open thr
close thr
caught
open iface
in iface
close iface
null ok
open loop
body
close loop
open loop
close loop
open loop
close loop
1
2
3
enum disposed
enum disposed
1
*/
import System;
using System;
using System.Collections.Generic;

class Res : IDisposable {
  String name;
  public Res(String n) { name = n; Console.WriteLine("open " + n); }
  public Void Dispose() { Console.WriteLine("close " + name); }
}
class CountEnum : IEnumerator<Int32> {
  Int32 i = 0; Int32 n;
  public CountEnum(Int32 n) { this.n = n; }
  public Boolean MoveNext() { if (i >= n) return false; i++; return true; }
  public Int32 Current { get { return i; } }
  public Void Reset() { i = 0; }
  public Void Dispose() { Console.WriteLine("enum disposed"); }
}
class Counter : IEnumerable<Int32> {
  public IEnumerator<Int32> GetEnumerator() { return new CountEnum(3); }
}
class Program {
 static Int32 Early() { using (Res r = new Res("early")) { return 5; } }
 static Void Thrower() { using (Res r = new Res("thr")) { throw new InvalidOperationException("x"); } }
 [EntryPoint] public static void Main() {
  using (Res a = new Res("a")) { Console.WriteLine("in a"); }
  using (Res b = new Res("b")) using (Res c = new Res("c")) { Console.WriteLine("in bc"); }
  Console.WriteLine(Early());
  try { Thrower(); } catch (InvalidOperationException e) { Console.WriteLine("caught"); }
  IDisposable d = new Res("iface");
  using (IDisposable e2 = d) { Console.WriteLine("in iface"); }
  using (Res n = null) { Console.WriteLine("null ok"); }
  for (Int32 k = 0; k < 3; k++) { using (Res l = new Res("loop")) { if (k == 1) continue; if (k == 2) break; Console.WriteLine("body"); } }
  foreach (Int32 v in new Counter()) { Console.WriteLine(v); }
  foreach (Int32 v in new Counter()) { if (v == 2) break; }
  List<Int32> li = new List<Int32>(); li.Add(1);
  foreach (Int32 v in li) Console.WriteLine(v);
 }
}
