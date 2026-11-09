// The result of an indexer that has an argument of a generic param is the object of a call, and the
// collection of a foreach: lookup[key].GetEnumerator(), foreach (T x in lookup[key]). A generic value
// is made a string (text + x): it is boxed, its ToString is called with the value.

/* EXPECT

pairs 1:1,1:3,0:2,0:4
direct 1,3
enumerator 2
done

*/

import System;

using System;
using System.Collections;
using System.Linq;

class Program
{
    // The same in an iterator.
    static IEnumerable<String> Pairs<T, TInner, TKey>(IEnumerable<T> outer, IEnumerable<TInner> inner,
        Delegate<TKey, T> outerKey, Delegate<TKey, TInner> innerKey)
    {
        ILookup<TKey, TInner> lookup = inner.ToLookup<TInner, TKey>(innerKey);

        foreach (T item in outer)
        {
            TKey key = outerKey(item);

            foreach (TInner match in lookup[key])
            {
                yield return "" + item + ":" + match;
            }
        }
    }

    static String Direct<TInner, TKey>(ILookup<TKey, TInner> lookup, TKey key)
    {
        String text = "";
        foreach (TInner x in lookup[key])
            text = text + (text == ""? "" : ",") + x;

        return text;
    }

    static Int32 First<TKey>(ILookup<TKey, Int32> lookup, TKey key)
    {
        IEnumerator<Int32> e = lookup[key].GetEnumerator();
        e.MoveNext();
        return e.Current;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32[] xs = new Int32[] { 1, 2, 3, 4 };

        String text = "";
        foreach (String s in Pairs(new Int32[] { 1, 0 }, xs, a => a, b => b % 2))
            text = text + (text == ""? "" : ",") + s;

        Console.WriteLine("pairs " + text);

        ILookup<Int32, Int32> lookup = xs.ToLookup<Int32, Int32>(x => x % 2);
        Console.WriteLine("direct " + Direct(lookup, 1));
        Console.WriteLine("enumerator " + First(lookup, 0));
        Console.WriteLine("done");
    }
}
