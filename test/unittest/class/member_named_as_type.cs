// A method that has the name of a type (public Res Res(...)): in the class, Res.Member is the member of the type, a
// method has no members.

/* EXPECT

0 9
3 0 7 5

*/

import System;

using System;

class Base { public Int32 V; }

class Res : Base
{
    public Res(Int32 v) { V = v; }
    public static Res Empty { get { return new Res(0); } }
    public static Int32 Count = 7;
    public Res Next() { return Res.Empty; }
    public Int32 Total() { return Res.Count + Res.Make(2).V; }
    public static Res Make(Int32 v) { return new Res(v); }
}

class Holder
{
    public Res Res(Int32 v) { Res r = new Res(v); return r; }
    public Res Other() { return Res.Empty; }
    public Int32 Count() { return Res.Count; }
    public static Res Maker(Int32 v) { return Res.Make(v); }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Res r = new Res(5);
        Console.WriteLine(r.Next().V + " " + r.Total());

        Holder h = new Holder();
        Console.WriteLine(h.Res(3).V + " " + h.Other().V + " " + h.Count() + " " + Holder.Maker(5).V);
    }
}
