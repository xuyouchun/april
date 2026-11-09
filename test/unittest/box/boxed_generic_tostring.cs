// A generic value that is made a string (text + item) is boxed, and the method of a primitive type
// that the virtual table of the box has (ToString) takes the value, not the reference of the box:
// it is so in the interpreter and in the native back end.

/* EXPECT

ints 1,2,3
longs 10000000000,-5
bools True,False
doubles 1.5,2.25
chars a,b
done

*/

import System;

using System;
using System.Collections;

class Program
{
    static String Join<T>(IEnumerable<T> items)
    {
        String text = "";
        foreach (T item in items)
            text = text + (text == ""? "" : ",") + item;

        return text;
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("ints " + Join(new Int32[] { 1, 2, 3 }));
        Console.WriteLine("longs " + Join(new Int64[] { 10000000000, -5 }));
        Console.WriteLine("bools " + Join(new Boolean[] { true, false }));
        Console.WriteLine("doubles " + Join(new Double[] { 1.5, 2.25 }));
        Console.WriteLine("chars " + Join(new Char[] { 'a', 'b' }));
        Console.WriteLine("done");
    }
}
