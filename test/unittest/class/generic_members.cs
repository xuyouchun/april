// Generic classes: a lambda and a method that is a delegate (the method that is called is the one
// of the instance that runs, Box<Int32>), a field of a generic param that is cast to Object
// (this.Value), and the members of a generic base class that are used by their names (fields,
// properties, methods and static fields), in a generic class and in a class that is not.

/* EXPECT

8
c 3
s 3
5
6
4
4
4
14
14
done

*/

import System;

using System;
using System.Collections;

class Box<T>
{
    public T Value;

    public Int32 Run(Int32 x)
    {
        Int32 y = 5;
        Delegate<Int32, Int32> f = a => a + y + x;
        return f(1);
    }

    public Int32 Twice(Int32 a) { return a * 2; }
    public Int32 Group(Int32 x) { Delegate<Int32, Int32> f = Twice; return f(x); }
    public static Int32 Plus(Int32 x) { Delegate<Int32, Int32> f = a => a + 1; return f(x); }

    public String Cast() { return "c " + (Object)this.Value; }
    public String Call() { return Show(this.Value); }
    public String Show(Object o) { return "s " + o; }
}

struct S
{
    public Int32 Run(Int32 x)
    {
        Delegate<Int32, Int32> f = a => a + x;
        return f(2);
    }
}

class Base<T>
{
    public T Value;
    public T Prop { get; set; }
    public static Int32 Count;
    public T Get() { return Value; }
    public Void Put(T x) { Value = x; }
    public static Int32 Twice(Int32 a) { return a * 2; }
}

class Derived<T> : Base<T>
{
    public T ViaMethod() { return Get(); }
    public Void Store(T x) { Put(x); Prop = x; }
    public T ViaProp() { return Prop; }
    public Int32 ViaStatic() { return Twice(Count); }
}

class Plain : Base<Int32>
{
    public Int32 Sum() { return Get() + Prop + Value + Count; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Box<Int32> b = new Box<Int32>();
        b.Value = 3;
        Console.WriteLine(b.Run(2));
        Console.WriteLine(b.Cast());
        Console.WriteLine(b.Call());
        Console.WriteLine(new S().Run(3));
        Console.WriteLine(b.Group(3));
        Console.WriteLine(Box<Int32>.Plus(1) + 2);

        Base<Int32>.Count = 7;
        Derived<Int32> d = new Derived<Int32>();
        d.Store(4);
        Console.WriteLine(d.ViaMethod());
        Console.WriteLine(d.ViaProp());
        Console.WriteLine(d.ViaStatic());

        Plain p = new Plain();
        p.Put(2);
        p.Prop = 3;
        Console.WriteLine(p.Sum());
        Console.WriteLine("done");
    }
}
