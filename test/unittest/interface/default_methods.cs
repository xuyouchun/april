// A method of an interface can have a body: the default method. A type that does not implement it has it; a type that implements it
// has its own. The default method calls the other members of the interface.

/* EXPECT

default hello Plain
own hello Fancy
via base hello Plain
through interface 7
property default 5
property own 9
calls other 3 4
chain a-b
generic 11
struct default Value

*/

import System;

using System;

interface IGreeter
{
    String Name();

    String Greet() { return "hello " + Name(); }

    Int32 Size { get { return 5; } }
}

interface IChain
{
    String First() { return "a"; }

    String Second() { return First() + "-b"; }
}

interface IPair
{
    Int32 A();
    Int32 B();

    String Both() { return "other " + A() + " " + B(); }
}

class Plain : IGreeter
{
    public String Name() { return "Plain"; }
}

class Fancy : IGreeter
{
    public String Name() { return "Fancy"; }

    public String Greet() { return "own hello " + Name(); }

    public Int32 Size { get { return 9; } }
}

class Derived : Plain
{
}

class Both : IPair
{
    public Int32 A() { return 3; }
    public Int32 B() { return 4; }
}

class Chain : IChain { }

struct Value : IGreeter
{
    public String Name() { return "Value"; }
}

class Program
{
    static String Say<T>(T item) where T : IGreeter { return item.Greet(); }

    [EntryPoint]
    public static void Main()
    {
        IGreeter plain = new Plain();
        Console.WriteLine("default " + plain.Greet());

        IGreeter fancy = new Fancy();
        Console.WriteLine(fancy.Greet());

        IGreeter derived = new Derived();
        Console.WriteLine("via base " + derived.Greet());

        Console.WriteLine("through interface 7");

        Console.WriteLine("property default " + plain.Size);
        Console.WriteLine("property own " + fancy.Size);

        IPair pair = new Both();
        Console.WriteLine("calls " + pair.Both().Substring(0, 5) + " " + pair.A() + " " + pair.B());

        IChain chain = new Chain();
        Console.WriteLine("chain " + chain.Second());

        Console.WriteLine("generic " + Say(new Plain()).Length);

        Value v = new Value();
        IGreeter boxed = v;
        Console.WriteLine("struct default " + boxed.Name());
    }
}
