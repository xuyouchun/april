// A constructor that calls another one: : this(...)

/* EXPECT

A(x,y)
A()
3
9
3
A(x,y)
A(x)
5
5

*/

import System;
using System;
class B { public int Base = 7; public B() { } public B(int x) { Base = x; } }
class A : B {
  public int X; public int Y; public int Z = 9;
  public A() : this(1, 2) { Console.WriteLine("A()"); }
  public A(int x) : this(x, 0) { Console.WriteLine("A(x)"); }
  public A(int x, int y) : base(x + y) { X = x; Y = y; Console.WriteLine("A(x,y)"); }
}
class Program { [EntryPoint] public static void Main() {
 A a = new A(); Console.WriteLine(a.X + a.Y); Console.WriteLine(a.Z); Console.WriteLine(a.Base);
 A b = new A(5); Console.WriteLine(b.X); Console.WriteLine(b.Base);
} }
