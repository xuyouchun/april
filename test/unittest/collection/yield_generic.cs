// The iterators that are generic methods (static methods of a class that is not generic): the
// state object is a generic class that has the generic params of the method.

/* EXPECT

twice 5 5
pick 1 2 3
where 3 4
select n1 n2 n3 n4
chain 20 40
break 1 2
zip 1a 2b
running 1 3 6
made
lazy 0
lazy 1
lazy 0 1

*/

import System;

using System;
using System.Collections;

public static class Tools
{
    // The param.
    public static IEnumerable<T> Twice<T>(T value)
    {
        yield return value;
        yield return value;
    }

    // A local variable that is not generic, and the items of a list.
    public static IEnumerable<T> Pick<T>(List<T> items)
    {
        Int32 index = 0;
        while (index < items.Count)
        {
            T item = items[index];
            yield return item;
            index++;
        }
    }

    // foreach in an iterator, a delegate that is a param.
    public static IEnumerable<T> Where<T>(this IEnumerable<T> source, Delegate<Boolean, T> predicate)
    {
        foreach (T item in source)
        {
            if (predicate(item))
                yield return item;
        }
    }

    public static IEnumerable<R> Select<T, R>(this IEnumerable<T> source, Delegate<R, T> selector)
    {
        foreach (T item in source)
        {
            R result = selector(item);
            yield return result;
        }
    }

    // yield break.
    public static IEnumerable<T> Take<T>(this IEnumerable<T> source, Int32 count)
    {
        Int32 taken = 0;
        foreach (T item in source)
        {
            if (taken >= count)
                yield break;

            taken++;
            yield return item;
        }
    }

    // Two generic params, and a delegate that has the items of both.
    public static IEnumerable<R> Zip<A, B, R>(IEnumerable<A> first, IEnumerable<B> second,
                                              Delegate<R, A, B> make)
    {
        IEnumerator<B> others = second.GetEnumerator();
        foreach (A a in first)
        {
            if (!others.MoveNext())
                yield break;

            B b = others.Current;
            yield return make(a, b);
        }
    }

    // A local variable of the generic param, assigned in a loop.
    public static IEnumerable<T> Running<T>(IEnumerable<T> source, T seed, Delegate<T, T, T> add)
    {
        T total = seed;
        foreach (T item in source)
        {
            total = add(total, item);
            yield return total;
        }
    }

    // The code before the first MoveNext is not run when the iterator is made.
    public static IEnumerable<Int32> Lazy<T>(T value)
    {
        Int32 n = 0;
        while (n < 2)
        {
            Console.WriteLine("lazy " + n);
            yield return n;
            n++;
        }
    }
}

class Program
{
    static void Show(String name, IEnumerable<Int32> items)
    {
        String text = name;
        foreach (Int32 item in items)
            text = text + " " + item;

        Console.WriteLine(text);
    }

    [EntryPoint]
    public static void Main()
    {
        List<Int32> list = new List<Int32>();
        list.Add(1); list.Add(2); list.Add(3); list.Add(4);

        Show("twice", Tools.Twice<Int32>(5));

        List<Int32> three = new List<Int32>();
        three.Add(1); three.Add(2); three.Add(3);
        Show("pick", Tools.Pick(three));

        Show("where", list.Where(v => v > 2));
        Console.WriteLine(Text("select", list.Select(v => "n" + v)));

        Show("chain", list.Where(v => v % 2 == 0).Select(v => v * 10));
        Show("break", list.Take(2));

        List<String> letters = new List<String>();
        letters.Add("a"); letters.Add("b");
        Console.WriteLine(Text("zip", Tools.Zip(list, letters, (n, c) => n + c)));

        Show("running", Tools.Running(three, 0, (a, b) => a + b));

        IEnumerable<Int32> lazy = Tools.Lazy<String>("x");
        Console.WriteLine("made");
        Show("lazy", lazy);
    }

    static String Text(String name, IEnumerable<String> items)
    {
        String text = name;
        foreach (String item in items)
            text = text + " " + item;

        return text;
    }
}
