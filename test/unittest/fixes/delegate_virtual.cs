// A delegate of a virtual method calls the implementation of the instance.

/* EXPECT

1
20
30
A
B

*/

import System;

using System;

class A
{
    public int Plain() { return 1; }
    public virtual int Value() { return 10; }
    public virtual String Name() { return "A"; }
}

class B : A
{
    public override int Value() { return 20; }
    public override String Name() { return "B"; }
}

class C : B
{
    public override int Value() { return 30; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        A a = new B();
        Delegate<int> plain = a.Plain;
        Delegate<int> value = a.Value;
        Console.WriteLine(plain());
        Console.WriteLine(value());

        A c = new C();
        Delegate<int> cv = c.Value;
        Console.WriteLine(cv());

        A x = new A();
        Delegate<String> xn = x.Name;
        Console.WriteLine(xn());

        Delegate<String> bn = a.Name;
        Console.WriteLine(bn());
    }
}
