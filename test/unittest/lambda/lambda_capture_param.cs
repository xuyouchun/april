// A lambda that modifies a param of the method shares it with the method (and with the other lambdas), like a local variable.

/* EXPECT

param 3
method sees 7
lambda sees 11 12
two lambdas 5 7
nested 6
string ab!
instance 4
mixed 7 2
done

*/

import System;

using System;

class Program
{
    int __base = 1;

    static Int32 Count(Int32 n)
    {
        Delegate<Void> inc = () => { n++; };
        inc();
        inc();
        inc();
        return n;
    }

    static Int32 Set(Int32 n)
    {
        Delegate<Void, Int32> put = x => { n = x; };
        put(7);
        return n;
    }

    static String Read(Int32 n)
    {
        Delegate<Int32> read = () => n;
        Int32 before = read();
        n = 12;
        return before + " " + read();
    }

    static String Two(Int32 total)
    {
        total = 0;
        Delegate<Void, Int32> add = x => { total += x; };
        Delegate<Int32> get = () => total;
        add(2);
        add(3);
        Int32 five = get();
        add(2);
        return five + " " + get();
    }

    static Int32 Nested(Int32 deep)
    {
        Delegate<Void> outer = () => {
            Delegate<Void> inner = () => { deep = deep + 5; };
            inner();
        };

        outer();
        return deep;
    }

    static String Text(String text)
    {
        Delegate<Void, String> append = s => { text = text + s; };
        append("b");
        append("!");
        return text;
    }

    Int32 Instance(Int32 n)
    {
        Delegate<Void> inc = () => { n = n + 1; };
        inc();
        return n + __base;
    }

    // A param and a local variable, one is not modified.
    static String Mixed(Int32 a, Int32 b)
    {
        Int32 c = 0;
        Delegate<Void> go = () => { a = a + b; c = c + 1; };
        go();
        go();
        return (a + c) + " " + b;
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("param " + Count(0));
        Console.WriteLine("method sees " + Set(0));
        Console.WriteLine("lambda sees " + Read(11));
        Console.WriteLine("two lambdas " + Two(100));
        Console.WriteLine("nested " + Nested(1));
        Console.WriteLine("string " + Text("a"));
        Console.WriteLine("instance " + new Program().Instance(2));
        Console.WriteLine("mixed " + Mixed(1, 2));
        Console.WriteLine("done");
    }
}
