// new T() of a generic param that has the new() or struct constraint: the object of the actual type, its constructor
// without params runs. A struct (or a number) is its default value.

/* EXPECT

class 4 7
two 3
list 0 1
dict 0
method 5 2
nested 6
twice 3 3
discard ok

*/

import System;

using System;
using System.Collections;

class Plain
{
    public Int32 A = 4;
    public Int32 B;

    public Plain() { B = 7; }
}

class Two
{
    public Int32 A;

    public Two() { A = 3; }

    public Two(Int32 a) { A = a; }
}

class Maker<X> where X : new()
{
    public X Make() { return new X(); }

    public X[] MakeMany(Int32 count)
    {
        X[] items = new X[count];
        for (Int32 k = 0; k < count; k++)
            items[k] = new X();

        return items;
    }
}

class Factory
{
    public static T Create<T>() where T : new() { return new T(); }

    public static T Fill<T>(Delegate<Void, T> setup) where T : new()
    {
        T value = new T();
        setup(value);
        return value;
    }
}

class Holder<T> where T : new()
{
    public T Item = new T();
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Plain p = new Maker<Plain>().Make();
        Console.WriteLine("class " + p.A + " " + p.B);

        Two t = new Maker<Two>().Make();
        Console.WriteLine("two " + t.A);

        List<Int32> list = new Maker<List<Int32>>().Make();
        list.Add(1);
        Console.WriteLine("list " + (list.Count - 1) + " " + list[0]);

        Dictionary<Int32, Int32> dict = new Maker<Dictionary<Int32, Int32>>().Make();
        Console.WriteLine("dict " + dict.Count);

        Plain f = Factory.Create<Plain>();
        Console.WriteLine("method " + (f.A + 1) + " " + (f.B - 5));

        Holder<Plain> holder = new Holder<Plain>();
        Console.WriteLine("nested " + (holder.Item.A + 2));

        Plain[] many = new Maker<Plain>().MakeMany(2);
        Console.WriteLine("twice " + (many[0].B - 4) + " " + (many[1].B - 4));

        new Maker<Plain>().Make();
        Console.WriteLine("discard ok");
    }
}
