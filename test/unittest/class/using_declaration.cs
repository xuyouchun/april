// The using declaration: using T obj = ...; , the object is disposed at the end of the block.

/* EXPECT

open a
body
close a
open b
open c
inner
close c
after
close b
open d
close d
open x
close x
caught
open e
open f
close f
close e
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
    static void Simple()
    {
        using IDisposable a = new Res("a");
        Console.WriteLine("body");
    }

    static void Nested()
    {
        using Res b = new Res("b");
        {
            using var c = new Res("c");
            Console.WriteLine("inner");
        }
        Console.WriteLine("after");
    }

    static int Early(bool flag)
    {
        using Res d = new Res("d");
        if (flag)
            return 1;

        Console.WriteLine("never");
        return 2;
    }

    static void Thrown()
    {
        try
        {
            using Res d = new Res("x");
            throw new Exception("boom");
        }
        catch (Exception)
        {
            Console.WriteLine("caught");
        }
    }

    static void Two()
    {
        using Res e = new Res("e");
        using Res f = new Res("f");
    }

    [EntryPoint]
    public static void Main()
    {
        Simple();
        Nested();
        Early(true);
        Thrown();
        Two();
        Console.WriteLine("done");
    }
}
