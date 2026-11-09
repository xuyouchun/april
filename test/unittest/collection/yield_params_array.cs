// The iterators that take a params array: the state object keeps the array, the items can be given
// one by one or as an array, in a method that is static, of an object, or generic.

/* EXPECT

1 2 3
none
a-b
x:7 8
arr 4 5
generic s t u
filtered 2 4

*/

import System;

using System;
using System.Collections;

public class Source
{
    public Int32 Offset;

    public Source(Int32 offset) { this.Offset = offset; }

    // An instance method: the items and a member.
    public IEnumerable<Int32> Shift(String tag, params Int32[] values)
    {
        foreach (Int32 value in values)
            yield return value + Offset;
    }
}

public static class Tools
{
    public static IEnumerable<Int32> Of(params Int32[] values)
    {
        foreach (Int32 value in values)
            yield return value;
    }

    // The params array is a param after another one.
    public static IEnumerable<String> Join(String separator, params String[] parts)
    {
        for (Int32 k = 0; k < parts.Length; k++)
        {
            if (k > 0)
                yield return separator;

            yield return parts[k];
        }
    }

    // A generic method.
    public static IEnumerable<T> Pick<T>(params T[] items)
    {
        foreach (T item in items)
            yield return item;
    }

    public static IEnumerable<Int32> Even(params Int32[] values)
    {
        foreach (Int32 value in values)
        {
            if (value % 2 == 0)
                yield return value;
        }
    }
}

public class Program
{
    [EntryPoint]
    public static void Main()
    {
        String text = "";

        foreach (Int32 v in Tools.Of(1, 2, 3))
            text += (text == ""? "" : " ") + v;

        Console.WriteLine(text);

        // No items.
        Int32 count = 0;
        foreach (Int32 v in Tools.Of())
            count++;

        Console.WriteLine(count == 0? "none" : "some");

        text = "";
        foreach (String s in Tools.Join("-", "a", "b"))
            text += s;

        Console.WriteLine(text);

        Source source = new Source(0);
        source = new Source(1);

        text = "";
        foreach (Int32 v in new Source(1).Shift("x", 6, 7))
            text += (text == ""? "x:" : " ") + v;

        Console.WriteLine(text);

        // The array is given as it is.
        Int32[] array = new Int32[] { 4, 5 };
        text = "";
        foreach (Int32 v in Tools.Of(array))
            text += (text == ""? "arr " : " ") + v;

        Console.WriteLine(text);

        text = "";
        foreach (String s in Tools.Pick<String>(new String[] { "s", "t", "u" }))
            text += (text == ""? "generic " : " ") + s;

        Console.WriteLine(text);

        text = "";
        foreach (Int32 v in Tools.Even(1, 2, 3, 4, 5))
            text += (text == ""? "filtered " : " ") + v;

        Console.WriteLine(text);
    }
}
