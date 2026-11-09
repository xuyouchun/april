// The fields and the methods of a base class are used by their names in a derived class.

/* EXPECT

4
12
8
done

*/

import System;

using System;

class Base
{
    public int V;
    protected int W = 5;
    public static int S = 8;

    public int M() { return 1; }
}

class Person : Base
{
    public int Show() { return V + M(); }

    public int Sum() { V = 7; return V + W; }

    public static int Static() { return S; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Person p = new Person();
        p.V = 3;
        Console.WriteLine(p.Show());
        Console.WriteLine(p.Sum());
        Console.WriteLine(Person.Static());
        Console.WriteLine("done");
    }
}
