// A generic method of a generic class that calls another generic method of the same class (Box<T>.Describe<R> calls Box<T>.Inner<R>,
// with the arguments written or inferred): the host of the called method has the arguments of the one that calls.

/* EXPECT

twice 3
inner 1 7 x
inner 1 7 5
5:7:3/7:7:3
two levels 3 7 [3]
done

*/

import System;

using System;

class Box<T>
{
    public T Value;

    public Box(T value)
    {
        Value = value;
    }

    public String Describe<R>(R other)
    {
        return Inner<R>(other, 1);
    }

    public String Inner<R>(R other, Int32 n)
    {
        return "inner " + n + " " + Value + " " + other;
    }

    public String Plain(Int32 n)
    {
        return Twice(n);
    }

    public String Twice(Int32 n)
    {
        return "twice " + n;
    }

    public String Chain<R>(R other)
    {
        return Level1<R>(other, 5) + "/" + Level1<R>(other, 7);
    }

    public String Level1<R>(R other, Int32 n)
    {
        return n + ":" + Value + ":" + Count<R>(other);
    }

    public String Describe2<R>(R other)
    {
        return "two levels " + Count<R>(other) + " " + Value + " [" + Count<Int32>(1) + "]";
    }

    public Int32 Count<R>(R other)
    {
        return 3;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Box<Int32> box = new Box<Int32>(7);
        Console.WriteLine(box.Plain(3));
        Console.WriteLine(box.Describe<String>("x"));
        Console.WriteLine(box.Describe(5));
        Console.WriteLine(new Box<Int32>(7).Chain("y"));
        Console.WriteLine(box.Describe2("z"));
        Console.WriteLine("done");
    }
}
