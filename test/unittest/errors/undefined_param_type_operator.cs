/* EXPECT_ERROR type "Foo" undefined */

// The operands of undefined type are reported once: the analysis goes on without crashing.

import System;

using System;

class Base
{
    public virtual int F(Foo x) { return x + 1; }
}

class Derived : Base
{
    public override int F(Foo x) { Foo y = x; return x + 1; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Base b = new Derived();
        Console.WriteLine(b.F(1));
    }
}
