// Extension methods: a static method of a static class with (this Type name) as first param is
// called as a method of the object, when the namespace of its class is used.

/* EXPECT

hello!
5
12
True
False
7
2
2
x-x-x
done

*/

import System;

using System;
using System.Collections;
using Tools;

namespace Tools
{
    public static class StringExtensions
    {
        public static String Shout(this String text)
        {
            return text + "!";
        }

        public static int Twice(this int value)
        {
            return value * 2;
        }

        public static bool IsBetween(this int value, int low, int high)
        {
            return value >= low && value <= high;
        }

        public static String Repeat(this String text, int count, String separator)
        {
            String result = text;
            for (int i = 1; i < count; i++)
                result = result + separator + text;

            return result;
        }
    }

    public static class ListExtensions
    {
        // A generic extension method: the type argument is inferred from the object.
        public static T First<T>(this List<T> items)
        {
            return items[0];
        }

        public static int Total<T>(this List<T> items)
        {
            return items.Count;
        }
    }
}

public class Counter
{
    public int Value = 3;
}

public static class CounterExtensions
{
    public static int Plus(this Counter counter, int n)
    {
        return counter.Value + n;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        String s = "hello";
        Console.WriteLine(s.Shout());

        // The arguments are after the object.
        int n = 6;
        Console.WriteLine(n.Twice() - 7);
        Console.WriteLine(n.Twice());
        Console.WriteLine(n.IsBetween(1, 10));
        Console.WriteLine(n.IsBetween(7, 10));

        // A call of an extension method on an expression, and a chain of them.
        Counter c = new Counter();
        Console.WriteLine(c.Plus(4));

        List<int> items = new List<int>();
        items.Add(2);
        items.Add(4);
        Console.WriteLine(items.First());
        Console.WriteLine(items.Total());

        Console.WriteLine("x".Repeat(3, "-"));

        // An extension method is a static method too.
        Console.WriteLine(StringExtensions.Twice(1) == 2 ? "done" : "failed");
    }
}
