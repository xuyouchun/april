// A typedef is another name of a type: typedef Name = Type; . The type that it names is written with the names that are seen
// where the typedef is declared (the namespaces that are used too), a number, an array, a generic type, an other typedef.
// The typedef is the type itself: it can be a base class, it makes objects, it has the static members.

/* EXPECT

3 q
hi
7
1
True
2
True
5
3

*/

import System;

using System;
using System.Collections;

typedef Num = Int32;
typedef Text = System.String;
typedef Short = int;
typedef Names = List<String>;
typedef Ints = Int32[];
typedef Again = Num;
typedef Lower = Base;

class Base
{
    public static Int32 Seven() { return 7; }

    public Int32 V = 1;
}

class Derived : Lower
{
}

class Program
{
    static void Hi() { Console.WriteLine("hi"); }

    [EntryPoint]
    public static void Main()
    {
        Num n = 3;
        Text s = "q";
        Console.WriteLine(n + " " + s);

        Delegate<Void> act = Hi;
        act();

        Console.WriteLine(Lower.Seven());

        Derived d = new Derived();
        Console.WriteLine(d.V);

        Lower b = d;
        Console.WriteLine(typeof(Lower).Equals(typeof(Base)));

        Names names = new Names();
        names.Add("a");
        names.Add("b");
        Console.WriteLine(names.Count);
        Console.WriteLine(typeof(Names).Equals(typeof(List<String>)));

        Ints ints = new Int32[5];
        Console.WriteLine(ints.Length);

        Again again = 3;
        Short sh = 0;
        Console.WriteLine(again + sh);
    }
}
