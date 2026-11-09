// The operand of a cast ends where its index, its call and its members end: (Double)items[k] is the
// cast of an item, not an index of the cast. And the elements of arrays that are read as another
// number type are converted.

/* EXPECT

3119
42
18119
3119.5
3
2
7
done

*/

import System;

using System;

class Program
{
    static Int64 Get(Int32 k)
    {
        return 40 + k;
    }

    [EntryPoint]
    public static void Main()
    {
        Int64[] longs = new Int64[] { 7, 3119 };
        Double[] doubles = new Double[] { 1.5, 2.75 };
        Int32 k = 1;

        Console.WriteLine((Double)longs[k]);
        Console.WriteLine((Double)Get(2));

        Double value = 1.5;
        value = value * 10000.0 + (Double)longs[k];
        Console.WriteLine(value);
        Console.WriteLine((Double)longs[k] + 0.5);

        Console.WriteLine((Int64)doubles[0] + 2);
        Console.WriteLine((Int64)(doubles[1]));

        Double d = longs[0];
        Console.WriteLine(d);
        Console.WriteLine("done");
    }
}
