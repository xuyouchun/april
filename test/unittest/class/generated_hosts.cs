// The classes that the compiler makes (the state of an iterator, the environment of a lambda) are
// nest types of the type that has the method, so the method can be in a class, a struct, a static
// class, a nest type, a generic class; and the iterator can be a generic method (an instance
// method, or with constraints), in a nest type, or in a generic class (a generic method of a
// generic class too).

/* EXPECT

7
7
7
7
9
1
2
a
a
r
r
-1
100
1
2
9
5
8
9
nested 6
w
1
2
done

*/

import System;

using System;
using System.Collections;

class Bag<T>
{
    public T Item;
    public Int32 Times;

    public IEnumerable<T> Repeat()
    {
        Int32 k = 0;
        while (k < Times)
        {
            yield return Item;
            k++;
        }
    }

    public IEnumerable<T> Twice(T other)
    {
        T local = other;
        yield return Item;
        yield return local;
    }

    public static IEnumerable<T> Pair(T a, T b)
    {
        yield return a;
        yield return b;
    }
}

class Util
{
    public Int32 Base = 100;

    public IEnumerable<T> Repeat<T>(T item, Int32 count)
    {
        Int32 k = 0;
        while (k < count)
        {
            yield return item;
            k++;
        }
    }

    public IEnumerable<Int32> Compare<T>(T a, T b) where T : IComparable<T>
    {
        yield return a.CompareTo(b);
        yield return Base;
    }

    public class Inner
    {
        public IEnumerable<T> Pair<T>(T a, T b)
        {
            yield return a;
            yield return b;
        }

        public static IEnumerable<T> One<T>(T a)
        {
            yield return a;
        }

        public Int32 Lambda(Int32 x)
        {
            Delegate<Int32, Int32> f = a => a * x;
            return f(2);
        }
    }
}

struct Point
{
    public Int32 X;

    public Int32 Shift(Int32 d)
    {
        Delegate<Int32, Int32> f = a => a + d;
        return f(X);
    }

    public IEnumerable<Int32> Twice()
    {
        yield return 1;
        yield return 2;
    }
}

static class Numbers
{
    public static IEnumerable<Int32> Upto(Int32 n)
    {
        Int32 k = 1;
        while (k <= n)
            yield return k++;
    }

    public static Int32 Apply(Int32 x)
    {
        Delegate<Int32, Int32> f = a => a + x;
        return f(1);
    }
}

class Box<U>
{
    public IEnumerable<T> With<T>(T extra)
    {
        yield return extra;
    }

    public static IEnumerable<T> Make<T>(T a, T b)
    {
        yield return a;
        yield return b;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Bag<Int32> bag = new Bag<Int32>();
        bag.Item = 7;
        bag.Times = 3;
        foreach (Int32 v in bag.Repeat()) Console.WriteLine(v);
        foreach (Int32 v in bag.Twice(9)) Console.WriteLine(v);
        foreach (Int32 v in Bag<Int32>.Pair(1, 2)) Console.WriteLine(v);

        Bag<String> text = new Bag<String>();
        text.Item = "a";
        text.Times = 2;
        foreach (String v in text.Repeat()) Console.WriteLine(v);

        Util u = new Util();
        foreach (String s in u.Repeat("r", 2)) Console.WriteLine(s);
        foreach (Int32 v in u.Compare(3, 5)) Console.WriteLine(v);

        Util.Inner inner = new Util.Inner();
        foreach (Int32 v in inner.Pair(1, 2)) Console.WriteLine(v);
        foreach (Int32 v in Util.Inner.One(9)) Console.WriteLine(v);

        Point p = new Point();
        p.X = 3;
        Int32 sum = 0;
        foreach (Int32 v in p.Twice()) sum += v;
        Console.WriteLine(p.Shift(2));
        Console.WriteLine(Numbers.Apply(4) + sum - 0);
        foreach (Int32 v in Numbers.Upto(3)) sum += v;
        Console.WriteLine(sum);
        Console.WriteLine("nested " + inner.Lambda(3));
        // A generic method of a generic class.
        foreach (String w in new Box<Int32>().With("w")) Console.WriteLine(w);
        foreach (Int32 v in Box<String>.Make(1, 2)) Console.WriteLine(v);
        Console.WriteLine("done");
    }
}
