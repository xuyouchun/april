// Lambdas whose params have no types: x => x + 1, (a, b) => a + b, the types are those of the delegate.

/* EXPECT

42
5
True

hi!
42
18
81
21
n5
101
*/
import System;
using System;
using System.Collections.Generic;

class Calc {
  public int Base = 10;
  public static int Apply(Delegate<int, int, int> f, int a, int b) { return f(a, b); }
  public static int Twice(Delegate<int, int> f, int v) { return f(f(v)); }
  public Delegate<int, int> Scaled() { return x => x * Base; }
}

class Program {
  [EntryPoint] public static void Main() {
    Delegate<int, int> inc = x => x + 1;
    Console.WriteLine(inc(41));
    Delegate<int, int, int> add = (a, b) => a + b;
    Console.WriteLine(add(2, 3));
    Delegate<Boolean, int> even = n => n % 2 == 0;
    Console.WriteLine(even(4));
    Delegate<Void, String> say = s => Console.WriteLine(s + "!");
    say("hi");
    Console.WriteLine(Calc.Apply((a, b) => a * b, 6, 7));
    Console.WriteLine(Calc.Twice(v => v * 3, 2));
    Delegate<int, int> blk = x => { return x * x; };
    Console.WriteLine(blk(9));
    Calc c = new Calc(); c.Base = 7;
    Delegate<int, int> sc = c.Scaled(); Console.WriteLine(sc(3));
    Delegate<String, int> name = n => "n" + n;
    Console.WriteLine(name(5));
    inc = x => x + 100;
    Console.WriteLine(inc(1));
  }
}
