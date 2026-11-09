// The object of a using needs no variable: using (new Res("a")) { ... } and using new Res("b"); ,
// the second is disposed at the end of the block.

/* EXPECT

open a
inside
close a
open b
open c
body
close c
close b
open d
close d
done

*/

import System;

using System;

class Res : IDisposable
{
    String name;

    public Res(String name)
    {
        this.name = name;
        Console.WriteLine("open " + name);
    }

    public void Dispose()
    {
        Console.WriteLine("close " + this.name);
    }
}

class Program
{
    static void Block()
    {
        using (new Res("a"))
        {
            Console.WriteLine("inside");
        }
    }

    static void Declared()
    {
        using new Res("b");
        using new Res("c");
        Console.WriteLine("body");
    }

    static int Early()
    {
        using new Res("d");
        return 1;
    }

    [EntryPoint]
    public static void Main()
    {
        Block();
        Declared();
        Early();
        Console.WriteLine("done");
    }
}
