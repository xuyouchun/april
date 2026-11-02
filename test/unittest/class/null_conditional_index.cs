// The null conditional index: a?[i], a?.b[i], a?[i].c. When a is null the value is null (a value
// is nullable), the rest of the chain is not evaluated.

/* EXPECT

7
-1
ab
True
2
-5
7
0
0
4
7

*/

import System;
using System;
class Node { public int[] Items; public String[] Names; public Node Next; public int N; }
class Program {
  static int Get(int[] a, int i) { return a?[i] ?? -1; }
  [EntryPoint] public static void Main() {
    int[] a = new int[3]; a[1] = 7;
    int[] none = null;
    Console.WriteLine(Get(a, 1));
    Console.WriteLine(Get(none, 1));
    String[] names = new String[2]; names[0] = "ab";
    String[] nn = null;
    Console.WriteLine(names?[0]);
    Console.WriteLine(nn?[0] == null);
    Console.WriteLine(names?[0].Length ?? -5);
    Console.WriteLine(nn?[0].Length ?? -5);
    Node n = new Node(); n.Items = a;
    Node m = null;
    Console.WriteLine(n?.Items[1] ?? 0);
    Console.WriteLine(m?.Items[1] ?? 0);
    Console.WriteLine(n.Items?[2] ?? 9);
    Console.WriteLine(m?.Next?.Items?[0] ?? 4);
    int? q = a?[1];
    Console.WriteLine(q.Value);
  }
}
