// An enum is boxed to an object, and unboxed with a cast, whatever its underlying type is.

/* EXPECT

True
True
5000000001
True
True

*/

import System;

using System;

enum Size { Small, Large }

enum Color : long { Red = 5000000000, Green }

class Program
{
    static Object Pass(Object o) { return o; }

    [EntryPoint]
    public static void Main()
    {
        Object boxed = Size.Large;
        Size back = (Size)boxed;
        Console.WriteLine(back == Size.Large);

        Object c = Color.Green;
        Console.WriteLine((Color)c == Color.Green);
        Console.WriteLine((long)(Color)c);

        Size s = (Size)Pass(Size.Small);
        Console.WriteLine(s == Size.Small);

        Object[] arr = new Object[2];
        arr[0] = Size.Large;
        Console.WriteLine((Size)arr[0] == Size.Large);
    }
}
