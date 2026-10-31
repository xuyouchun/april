// Operators defined by the types: classes and structs.

/* EXPECT

4
6
2
6
False
True
True
False
-1
4
7
15
False
True
3
False
True
3
2
True
True
3
5

*/

import System;

using System;

class V {
  public int X; public int Y;
  public V(int x, int y) { X = x; Y = y; }
  public static V operator +(V a, V b) { return new V(a.X + b.X, a.Y + b.Y); }
  public static V operator -(V a, V b) { return new V(a.X - b.X, a.Y - b.Y); }
  public static V operator *(V a, int k) { return new V(a.X * k, a.Y * k); }
  public static bool operator ==(V a, V b) { return a.X == b.X && a.Y == b.Y; }
  public static bool operator !=(V a, V b) { return !(a == b); }
  public static bool operator <(V a, V b) { return a.X < b.X; }
  public static bool operator >(V a, V b) { return a.X > b.X; }
  public static V operator -(V a) { return new V(-a.X, -a.Y); }
}
struct M {
  public int V;
  public M(int v) { V = v; }
  public static M operator +(M a, M b) { return new M(a.V + b.V); }
  public static M operator *(int k, M a) { return new M(a.V * k); }
  public static bool operator ==(M a, M b) { return a.V == b.V; }
  public static bool operator !=(M a, M b) { return a.V != b.V; }
  public static M operator ++(M a) { return new M(a.V + 1); }
  public static bool operator !(M a) { return a.V == 0; }
}
class C {
  public int N;
  public static C operator /(C a, C b) { C r = new C(); r.N = a.N / b.N; return r; }
  public static C operator %(C a, int b) { C r = new C(); r.N = a.N % b; return r; }
}
class Program
{
    [EntryPoint]
    public static void Main()
    {
        {
 V a = new V(1, 2); V b = new V(3, 4);
 V c = a + b; Console.WriteLine(c.X); Console.WriteLine(c.Y);
 V d = b - a; Console.WriteLine(d.X);
 V e = a * 3; Console.WriteLine(e.Y);
 Console.WriteLine(a == b); Console.WriteLine(a != b); Console.WriteLine(a < b); Console.WriteLine(a > b);
 V f = -a; Console.WriteLine(f.X);
 a += b; Console.WriteLine(a.X);
        }

        {
 M a = new M(2); M b = new M(5);
 M c = a + b; Console.WriteLine(c.V);
 M d = 3 * b; Console.WriteLine(d.V);
 Console.WriteLine(a == b); Console.WriteLine(a != b);
 M e = a; e++; Console.WriteLine(e.V);
 Console.WriteLine(!a); Console.WriteLine(!new M(0));
 C x = new C(); x.N = 17; C y = new C(); y.N = 5;
 C z = x / y; Console.WriteLine(z.N);
 C w = x % 5; Console.WriteLine(w.N);
 C n = null;
 Console.WriteLine(n == null);
 Console.WriteLine(x != null);

            M s = new M(1);
            s += new M(2);
            Console.WriteLine(s.V);

            V p = new V(1, 1);
            p += new V(4, 4);
            Console.WriteLine(p.X);
        }
    }
}
