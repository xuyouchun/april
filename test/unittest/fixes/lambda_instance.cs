// Lambdas of instance methods use this, and a method of the object as a delegate with no object in front.

/* EXPECT

42
5
hello
7
81
18
101
105
*/
import System;
using System;
class Program {
 static int Twice(Delegate<int, int> f, int v) { return f(f(v)); }
 int bias = 100;
 int Instance(int x) { Delegate<int, int> g = (int a) => a + bias; return g(x); }
 [EntryPoint] public static void Main() {
  Delegate<int, int> inc = (int x) => x + 1;
  Console.WriteLine(inc(41));
  Delegate<int, int, int> add = (int a, int b) => a + b;
  Console.WriteLine(add(2, 3));
  Delegate<Void, String> say = (String s) => Console.WriteLine(s);
  say("hello");
  Delegate<int> k = () => 7;
  Console.WriteLine(k());
  Delegate<int, int> sq = (int x) => { return x * x; };
  Console.WriteLine(sq(9));
  Console.WriteLine(Twice((int x) => x * 3, 2));
  inc = (int x) => x + 100;
  Console.WriteLine(inc(1));
  Program p = new Program();
  Console.WriteLine(p.Instance(5));
 }
}
