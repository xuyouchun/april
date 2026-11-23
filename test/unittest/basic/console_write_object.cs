// Console.Write / WriteLine take an enum, a struct or an object: its text is the one of ToString (IStringable). A null string is an empty line.

/* EXPECT

Y
S3
S3
1
x

after
5

|5
Y

*/

import System;

using System;

enum E { X, Y }

struct S : IStringable
{
    public Int32 v;

    public String ToString()
    {
        return "S" + v;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        S s = new S();
        s.v = 3;
        E e = E.Y;

        Console.WriteLine(e);
        Console.WriteLine(s);

        Object o = s;
        Console.WriteLine(o);
        Console.WriteLine(1);
        Console.WriteLine("x");

        String? n = null;
        Console.WriteLine(n);
        Console.WriteLine("after");

        // A nullable: the text of the value, an empty line when there is none.
        Int32? five = 5;
        Int32? none = null;
        Console.WriteLine(five);
        Console.WriteLine(none);
        Console.WriteLine("|" + five);
        E? y = E.Y;
        Console.WriteLine(y);
    }
}
