// An iterator (yield) in a part of a partial class that is not the first one.

/* EXPECT

0
2
4
7
8
done

*/

import System;

using System;
using System.Collections;

partial class Seq
{
    public int Step = 2;

    public static IEnumerable<Int32> Range(Int32 lo, Int32 hi)
    {
        for (Int32 i = lo; i < hi; i++)
            yield return i;
    }
}

partial class Seq
{
    public IEnumerable<Int32> Evens(Int32 count)
    {
        for (Int32 i = 0; i < count; i++)
            yield return i * this.Step;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Seq seq = new Seq();

        foreach (Int32 v in seq.Evens(3))
            Console.WriteLine(v);

        foreach (Int32 v in Seq.Range(7, 9))
            Console.WriteLine(v);

        Console.WriteLine("done");
    }
}
