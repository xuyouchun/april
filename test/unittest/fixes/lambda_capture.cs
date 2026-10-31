// Lambdas that use the local variables and the params of the method that has them: the lambda has
// copies of their values, in the object of its delegate. The delegates are kept in lists.

/* EXPECT

6|8
42
30
9
1
2
5
aa
bb
5
9
*/
import System;
using System;
using System.Collections.Generic;

class Calc {
    public static Delegate<Int32, Int32> Adder(Int32 n) { return x => x + n; }
    public static Delegate<Int32, Int32> Curry(Int32 a) { return b => a * 10 + b; }
    public static Delegate<Int32, Int32, Int32> Mul(Int32 k) { return (x, y) => x * y * k; }
    public Int32 Base = 100;
    public Delegate<Int32, Int32> Inst(Int32 d) { return x => x + d; }
}

class Program {
    [EntryPoint] public static void Main() {
        var add5 = Calc.Adder(5);
        var add7 = Calc.Adder(7);
        Console.WriteLine(add5(1) + "|" + add7(1));
        var cu = Calc.Curry(4); Console.WriteLine(cu(2));
        var mu = Calc.Mul(3); Console.WriteLine(mu(2, 5));
        var c = new Calc();
        var ins = c.Inst(8); Console.WriteLine(ins(1));

        List<Delegate<Int32>> fs = new List<Delegate<Int32>>();
        for (Int32 i = 0; i < 3; i++) {
            Int32 j = i * i;
            fs.Add(() => j + 1);
        }
        foreach (var f in fs) Console.WriteLine(f());

        String[] names = new String[] { "a", "b" };
        var gs = new List<Delegate<String>>();
        foreach (var n in names) gs.Add(() => n + n);
        foreach (var g in gs) Console.WriteLine(g());

        Int32 total = 0;
        Int32 z = 4;
        Delegate<Void, Int32> pr = v => Console.WriteLine(v + z);
        pr(1);
        Delegate<Int32, Int32> twice = v => v + z * 2;
        Console.WriteLine(twice(1));
    }
}
