// Default values of params, and params arrays.

/* EXPECT

6
3
123
923
983
987
6
0
7
9
a,b,c

4
30
6

*/

import System;

using System;

class A {
  public static int F(int a, int b = 5) { return a + b; }
  public static int G(int a = 1, int b = 2, int c = 3) { return a * 100 + b * 10 + c; }
  public static int Sum(params int[] xs) { int s = 0; foreach (int x in xs) s += x; return s; }
  public static String Join(String sep, params String[] parts) {
    String r = "";
    for (int i = 0; i < parts.Length; i++) { if (i > 0) r += sep; r += parts[i]; }
    return r;
  }
  public static int Count(params int[] xs) { return xs.Length; }
  public int M(int x, int y = 10) { return x * y; }
}
class Program
{
    [EntryPoint]
    public static void Main()
    {
 Console.WriteLine(A.F(1)); Console.WriteLine(A.F(1, 2));
 Console.WriteLine(A.G()); Console.WriteLine(A.G(9)); Console.WriteLine(A.G(9, 8)); Console.WriteLine(A.G(9, 8, 7));
 Console.WriteLine(A.Sum(1, 2, 3)); Console.WriteLine(A.Sum()); Console.WriteLine(A.Sum(7));
 int[] arr = new int[] { 4, 5 };
 Console.WriteLine(A.Sum(arr));
 Console.WriteLine(A.Join(",", "a", "b", "c")); Console.WriteLine(A.Join("-"));
 Console.WriteLine(A.Count(1, 2, 3, 4));
 A a = new A(); Console.WriteLine(a.M(3)); Console.WriteLine(a.M(3, 2));
    }
}
