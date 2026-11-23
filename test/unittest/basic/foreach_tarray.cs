// foreach goes through an Array<T> (the base type of the arrays) as it goes through an array.

/* EXPECT

8
a b
0

*/

import System;

using System;

class Program
{
    static Array<Int32> Numbers()
    {
        Int32[] a = new Int32[3];
        a[0] = 3;
        a[2] = 5;
        return a;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32 sum = 0;
        foreach (Int32 x in Numbers())
            sum += x;
        Console.WriteLine(sum);

        String[] names = new String[2];
        names[0] = "a";
        names[1] = "b";
        Array<String> list = names;
        String text = "";
        foreach (String s in list)
            text = text.Length == 0? s : text + " " + s;
        Console.WriteLine(text);

        Array<Int32> empty = new Int32[0];
        Int32 count = 0;
        foreach (Int32 x in empty)
            count++;
        Console.WriteLine(count);
    }
}
