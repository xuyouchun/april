// Abstract members and interface members that are implemented: no error.

/* EXPECT

11
20
3
5

*/

import System;

using System;

interface IA
{
    int A(int x);
}

interface IB : IA
{
    int B();
}

abstract class Base
{
    public abstract int F(int x);
}

// An abstract class does not need to implement what it inherits; it declares the members of
// its interfaces that it does not implement.
abstract class Middle : Base, IB
{
    public int A(int x) { return x * 2; }
    public abstract int B();
}

class Leaf : Middle
{
    public override int F(int x) { return x + 1; }
    public override int B() { return 3; }
}

// Implemented by a base class.
class Impl
{
    public int B() { return 5; }
    public int A(int x) { return x; }
}

class Derived : Impl, IB
{
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Leaf leaf = new Leaf();
        Base b = leaf;
        IB ib = leaf;

        Console.WriteLine(b.F(10));
        Console.WriteLine(ib.A(10));
        Console.WriteLine(ib.B());

        IB ib2 = new Derived();
        Console.WriteLine(ib2.B());
    }
}
