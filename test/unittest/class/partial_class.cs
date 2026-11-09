// partial types in a file: the parts are one type, with the members of all of them.

/* EXPECT

10
20
30
hello Tom
3
7
base 5
inner 42
iface 9
struct 11
gen 1,2
done

*/

import System;

using System;

interface IShow
{
    int Show();
}

class Base
{
    public int V;
}

partial class Counter
{
    private int _n;

    public Counter(int n) { _n = n; }
}

partial class Counter
{
    public int Next() { _n += 10; return _n; }
}

public partial class Counter
{
    public int Peek { get { return _n; } }
}

// A part has the base class, another has an interface.
partial class Person : Base
{
    public String Name;
}

partial class Person : IShow
{
    public int Show() { return this.V + 4; }

    public String Hello() { return "hello " + Name; }
}

// Nested partial types.
partial class Outer
{
    public partial class Inner
    {
        public int A = 40;
    }
}

partial class Outer
{
    public partial class Inner
    {
        public int B = 2;
        public int Sum() { return A + B; }
    }

    public int Run() { return new Inner().Sum(); }
}

partial struct Pair
{
    public int X;
}

partial struct Pair
{
    public int Y;

    public int Total() { return X + Y; }
}

partial class Box<T>
{
    public T First;
}

partial class Box<T>
{
    public T Second;

    public T Pick(bool first) { if (first) return First; return Second; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Counter c = new Counter(0);
        Console.WriteLine(c.Next());
        Console.WriteLine(c.Next());
        Console.WriteLine(c.Peek + c.Next() - c.Peek + 10);

        Person p = new Person();
        p.Name = "Tom";
        Console.WriteLine(p.Hello());

        Console.WriteLine(new Outer().Run() - 39);

        p.V = 3;
        Console.WriteLine(p.Show());

        Base b = p;
        b.V = 5;
        Console.WriteLine("base " + b.V);

        Console.WriteLine("inner " + new Outer().Run());

        IShow show = p;
        p.V = 5;
        Console.WriteLine("iface " + show.Show());

        Pair pair = new Pair();
        pair.X = 4;
        pair.Y = 7;
        Console.WriteLine("struct " + pair.Total());

        Box<int> box = new Box<int>();
        box.First = 1;
        box.Second = 2;
        int g1 = box.Pick(true);
        int g2 = box.Pick(false);
        Console.WriteLine("gen " + g1 + "," + g2);

        Console.WriteLine("done");
    }
}
