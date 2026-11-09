// The branches of a condition expression have the type of a generic param.

/* EXPECT

1
2
a-b
done

*/

import System;

using System;

class Box<T>
{
    public T First;
    public T Second;

    public T Pick(bool first) { return first? First : Second; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Box<int> b = new Box<int>();
        b.First = 1;
        b.Second = 2;

        int x = b.Pick(true);
        int y = b.Pick(false);
        Console.WriteLine(x);
        Console.WriteLine(y);

        Box<String> s = new Box<String>();
        s.First = "a";
        s.Second = "b";
        Console.WriteLine(s.Pick(true) + "-" + s.Pick(false));

        Console.WriteLine("done");
    }
}
