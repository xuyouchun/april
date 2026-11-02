// EXPECT_WARNING none

/* EXPECT

5
two
3
2
4
2
3
0

*/

#nullable enable
import System;
using System;
using System.Collections.Generic;
class Node { public String? Name; public Node? Next; public int N;
  public String? Cached;
  public int Len() {
    if (Cached == null) { Cached = "abc"; }
    return Cached.Length;
  }
  public int Len2() {
    if (this.Name != null) return this.Name.Length;
    return 0;
  }
  public static Node? Make(int k) { if (k > 0) return new Node(); return null; }
}
class Program {
  static int Sum(List<String> items) {
    int total = 0;
    foreach (String s in items) { total += s.Length; }
    return total;
  }
  static String Pick(int k, String? a) {
    String r;
    switch (k) {
      case 1: r = "one"; break;
      case 2: r = a ?? "two"; break;
      default: r = "many"; break;
    }
    return r;
  }
  static int Safe(String? s) {
    try {
      if (s == null) throw new Exception("x");
      return s.Length;
    } catch (Exception) {
      return -1;
    }
  }
  static int ForLoop(Node? head) {
    int c = 0;
    for (Node? p = head; p != null; p = p.Next) { c++; }
    return c;
  }
  static int Lam(String? s) {
    int k = 0;
    if (s != null) { k = s.Length; }
    Delegate<int, int> f = (int x) => x + 1;
    return f(k);
  }
  static int DoLoop(Node? n) {
    int c = 0;
    do { if (n == null) break; c++; n = n.Next; } while (n != null);
    return c;
  }
  [EntryPoint] public static void Main() {
    List<String> l = new List<String>(); l.Add("ab"); l.Add("cde");
    Console.WriteLine(Sum(l));
    Console.WriteLine(Pick(2, null));
    Console.WriteLine(Safe("abc"));
    Node h = new Node(); h.Next = new Node();
    Console.WriteLine(ForLoop(h));
    Console.WriteLine(Lam("xyz"));
    Console.WriteLine(DoLoop(h));
    Node? m = Node.Make(1);
    if (m != null) Console.WriteLine(m.Len());
    Console.WriteLine(h.Len2());
  }
}
