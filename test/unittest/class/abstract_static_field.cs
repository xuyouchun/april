// An abstract class that has a static field (it has the data of its statics, though it has no object): the
// native programs do not make its abstract methods, the slots of them are empty.

/* EXPECT

6
4
impl
same True

*/

import System;

using System;

abstract class Base
{
    protected Base(String name)
    {
        Name = name;
    }

    public String Name;

    public abstract Int32 Count(String s);

    public Int32 Twice(String s) => Count(s) * 2;

    public static Base Shared
    {
        get
        {
            if (__shared == null)
                __shared = new Impl();

            return __shared;
        }
    }

    private static Base? __shared;
}

class Impl : Base
{
    public Impl() : base("impl")
    {
    }

    public override Int32 Count(String s) => s.Length;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Base b = Base.Shared;
        Console.WriteLine(b.Twice("abc"));
        Console.WriteLine(b.Count("abcd"));
        Console.WriteLine(b.Name);
        Console.WriteLine("same " + Object.ReferenceEquals(b, Base.Shared));
    }
}
