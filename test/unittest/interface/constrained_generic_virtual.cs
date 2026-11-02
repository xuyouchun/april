// A virtual method of a generic class that is overridden, called through a generic parameter
// that has an interface or the generic class as a constraint: the method of the type that the
// parameter is, through the virtual table. A generic class and a generic struct that have
// the constraint call it too.

/* EXPECT

1
7
7
7
7
3
8
6

*/

import System;

using System;

interface IShape
{
    int Area();
}

class Base<T> : IShape
{
    public T Value;

    public virtual int Area() { return 1; }
}

class Derived : Base<int>
{
    public override int Area() { return 7; }
}

struct Cell<T> : IShape
{
    public T Value;

    public int Area() { return 3; }
}

class Holder<T> where T : IShape
{
    public int Run(T t) { return t.Area() + 1; }
}

class Util
{
    public static int ByInterface<S>(S s) where S : IShape
    {
        return s.Area();
    }

    public static int ByBase<S>(S s) where S : Base<int>
    {
        return s.Area();
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Util.ByInterface<Base<int>>(new Base<int>()));
        Console.WriteLine(Util.ByInterface<Derived>(new Derived()));
        Console.WriteLine(Util.ByBase<Base<int>>(new Derived()));
        Console.WriteLine(Util.ByBase<Derived>(new Derived()));

        Holder<Derived> holder = new Holder<Derived>();
        Console.WriteLine(holder.Run(new Derived()) - 1);

        Cell<String> cell = new Cell<String>();
        Console.WriteLine(Util.ByInterface<Cell<String>>(cell));

        Holder<Derived> h2 = new Holder<Derived>();
        Console.WriteLine(h2.Run(new Derived()));

        Holder<Cell<int>> h3 = new Holder<Cell<int>>();
        Cell<int> c = new Cell<int>();
        Console.WriteLine(h3.Run(c) + 2);
    }
}
