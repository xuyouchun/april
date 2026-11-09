// The type arguments of a generic method are inferred from the interfaces that the argument
// implements, those of its base types too: List<T> is an IEnumerable<T> by IList<T> and by
// IReadOnlyList<T>, that is one way to infer T.

/* EXPECT

3
2
1
1
a
3
4

*/

import System;

using System;
using System.Collections;

public static class Tools
{
    public static Int32 CountOf<T>(this IEnumerable<T> source)
    {
        Int32 n = 0;
        foreach (T item in source)
            n++;

        return n;
    }

    public static T FirstOf<T>(this IEnumerable<T> source)
    {
        foreach (T item in source)
            return item;

        return default(T);
    }

    public static Int32 SizeOf<T>(this ICollection<T> source)
    {
        return source.Count;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        List<Int32> list = new List<Int32>();
        list.Add(7); list.Add(8); list.Add(9);

        // By the interfaces of a class, as a method and as an extension method.
        Console.WriteLine(Tools.CountOf(list));
        Console.WriteLine(list.CountOf() - 1);

        Queue<Int32> queue = new Queue<Int32>();
        queue.Enqueue(5);
        Console.WriteLine(queue.CountOf());

        Stack<Int32> stack = new Stack<Int32>();
        stack.Push(6);
        Console.WriteLine(stack.CountOf());

        // A dictionary is an IEnumerable<KeyValuePair<TKey, TValue>>.
        Dictionary<String, Int32> dictionary = new Dictionary<String, Int32>(new StringComparer());
        dictionary.Add("a", 1);
        Console.WriteLine(dictionary.FirstOf().Key);

        // An interface that is not the nearest one.
        Console.WriteLine(list.SizeOf());

        // The argument is the interface.
        IEnumerable<Int32> e = list;
        Console.WriteLine(e.CountOf() + 1);
    }
}
