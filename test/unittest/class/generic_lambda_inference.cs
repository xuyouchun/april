// The lambdas that are given to generic methods: the types of the params of a lambda are those of
// the delegate (T is known from the other arguments), and the return type of the delegate (R) is
// the type of the body of the lambda.

/* EXPECT

2
2
2
2
n1
n1
10
6
123
7
2

*/

import System;

using System;
using System.Collections;

public static class Tools
{
    public static Int32 CountIf<T>(this IEnumerable<T> source, Delegate<Boolean, T> predicate)
    {
        Int32 n = 0;
        foreach (T item in source)
        {
            if (predicate(item))
                n++;
        }

        return n;
    }

    public static R MapFirst<T, R>(this IEnumerable<T> source, Delegate<R, T> selector)
    {
        foreach (T item in source)
        {
            R result = selector(item);
            return result;
        }

        return default(R);
    }

    public static R Fold<T, R>(this IEnumerable<T> source, R seed, Delegate<R, R, T> func)
    {
        R accumulated = seed;
        foreach (T item in source)
            accumulated = func(accumulated, item);

        return accumulated;
    }

    // A generic value that a getter returns (list[0], the items are T).
    public static T FirstOf<T>(List<T> source)
    {
        T first = source[0];
        return first;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        List<Int32> list = new List<Int32>();
        list.Add(1); list.Add(2); list.Add(3);

        // T is given, inferred from the receiver, and from the explicit type of the param.
        Console.WriteLine(list.CountIf<Int32>(v => v > 1));
        Console.WriteLine(list.CountIf(v => v > 1));
        Console.WriteLine(Tools.CountIf(list, v => v > 1));
        Console.WriteLine(list.CountIf((Int32 v) => v > 1));

        // R is the type of the body.
        Console.WriteLine(list.MapFirst<Int32, String>(v => "n" + v));
        Console.WriteLine(list.MapFirst(v => "n" + v));
        Console.WriteLine(list.MapFirst((Int32 v) => v * 10));

        // R is given by an argument too.
        Console.WriteLine(list.Fold(0, (a, v) => a + v));
        Console.WriteLine(list.Fold("", (a, v) => a + v));

        // A local variable of T that a getter gives its value.
        List<Int32> one = new List<Int32>();
        one.Add(7);
        Console.WriteLine(Tools.FirstOf(one));

        // A lambda that has two statements.
        Console.WriteLine(list.CountIf(v => { Int32 w = v * 2; return w > 3; }));
    }
}
