// The elements of an array initializer are converted to the type of the elements: a negative Int32
// is an Int64 with its sign, a Char takes the size of a char of the runtime.

/* EXPECT

int64 10000000000,-2,-7,5
char a,b,c
int8 1,-2
int16 1,-2,3
uint64 1,2
object 1,2.5
done

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 k = -7;
        Int64[] longs = new Int64[] { 10000000000, -2, k, 5 };
        Console.WriteLine("int64 " + longs[0] + "," + longs[1] + "," + longs[2] + "," + longs[3]);

        Char[] chars = new Char[] { 'a', 'b', 'c' };
        Console.WriteLine("char " + chars[0] + "," + chars[1] + "," + chars[2]);

        Int8[] bytes = new Int8[] { 1, -2 };
        Console.WriteLine("int8 " + bytes[0] + "," + bytes[1]);

        Int16[] shorts = new Int16[] { 1, -2, 3 };
        Console.WriteLine("int16 " + shorts[0] + "," + shorts[1] + "," + shorts[2]);

        UInt64[] ulongs = new UInt64[] { 1, 2 };
        Console.WriteLine("uint64 " + ulongs[0] + "," + ulongs[1]);

        Object[] objects = new Object[] { 1, 2.5 };
        Console.WriteLine("object " + objects[0] + "," + objects[1]);

        Console.WriteLine("done");
    }
}
