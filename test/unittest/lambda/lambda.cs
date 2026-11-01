// Lambdas: (int a, int b) => a + b, with an expression or a block as the body, converted to a
// delegate that is assigned, passed or returned.

/* EXPECT

2
21
21
abab!
11
12
n5
5
True

False

nested
10
20
30

*/

import System;

using System;
using System.Collections.Generic;

struct Pt
{
    public int X;
    public int Y;
}

class Calc
{
    public int Base = 10;
    public static Delegate<int, int> Adder(int unused)
    {
        return (int x) => x + 1;
    }

    public Delegate<int, int> Scaled()
    {
        return (int x) => x * Base;
    }

    public static int Apply(Delegate<int, int, int> f, int a, int b)
    {
        return f(a, b);
    }

    public static String Join(Delegate<String, String> f, String s)
    {
        return f(s) + "!";
    }
}

class Program
{
    static int Counter = 0;
    static Delegate<int, int> Make()
    {
        return (int a) =>
        {
            Counter++;
            return a + Counter;
        };
    }

    [EntryPoint]
    public static void Main()
    {
        Delegate<int, int> f = Calc.Adder(0);
        Console.WriteLine(f(1));
        Calc c = new Calc();
        c.Base = 7;
        Delegate<int, int> g = c.Scaled();
        Console.WriteLine(g(3));
        Console.WriteLine(Calc.Apply((int a, int b) => a * b + 1, 4, 5));
        Console.WriteLine(Calc.Join((String s) => s + s, "ab"));
        Delegate<int, int> m = Make();
        Console.WriteLine(m(10));
        Console.WriteLine(m(10));
        Delegate<String, int> name = (int n) => "n" + n;
        Console.WriteLine(name(5));
        Delegate<Pt, int, int> mk = (int a, int b) =>
        {
            Pt p = new Pt();
            p.X = a;
            p.Y = b;
            return p;
        };
        Pt q = mk(2, 3);
        Console.WriteLine(q.X + q.Y);
        Delegate<Boolean, int> even = (int n) => n % 2 == 0;
        Console.WriteLine(even(4));
        Console.WriteLine(even(5));
        Delegate<Delegate<int, int>, int> curry = (int a) => (int b) => b;
        Console.WriteLine("nested");
        List<Int32> list = new List<Int32>();
        list.Add(1);
        list.Add(2);
        list.Add(3);
        Int32 total = 0;
        Delegate<Void, Int32> add = (Int32 v) => Console.WriteLine(v * 10);
        foreach (Int32 v in list)
            add(v);
    }
}
