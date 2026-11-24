// A throw expression is the right operand of ??, or a branch of ?: : the exception is thrown when the value is needed.

/* EXPECT

ok 5
null ArgumentException
ref name
ref null InvalidOperationException
cond 7
cond bad InvalidOperationException
branch1 ok
branch2 ok
nullable 3
nullable none InvalidOperationException
ctor ok
ctor ArgumentException
chain ok

*/

import System;

using System;

class Person
{
    public String Name;

    public Person(String name)
    {
        Name = name ?? throw new ArgumentException("name");
    }
}

class Program
{
    static String Required(String value)
    {
        return value ?? throw new InvalidOperationException("required");
    }

    static Int32 Parse(Int32 k)
    {
        return k > 0? k : throw new InvalidOperationException("bad");
    }

    static String Branch(Boolean first)
    {
        return first? throw new InvalidOperationException("first") : "second";
    }

    static Int32 Value(Int32? n)
    {
        return n ?? throw new InvalidOperationException("none");
    }

    static String Try(Func<String> f)
    {
        try
        {
            return f();
        }
        catch (Exception e)
        {
            return e.GetType().Name;
        }
    }

    static Int32 TryInt(Func<Int32> f)
    {
        try
        {
            return f();
        }
        catch (Exception e)
        {
            Console.Write(e.GetType().Name + " ");
            return -1;
        }
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("ok " + 5);

        Console.WriteLine("null " + Try(() => new Person(null).Name));
        Console.WriteLine("ref " + Required("name"));
        Console.WriteLine("ref null " + Try(() => Required(null)));

        Console.WriteLine("cond " + Parse(7));
        Console.WriteLine("cond bad " + Try(() => Parse(0).ToString()));

        Console.WriteLine("branch1 " + (Branch(false) == "second"? "ok" : "?"));
        Console.WriteLine("branch2 " + (Try(() => Branch(true)) == "InvalidOperationException"? "ok" : "?"));

        Console.WriteLine("nullable " + Value(3));
        Console.WriteLine("nullable none " + Try(() => Value(null).ToString()));

        Console.WriteLine("ctor " + (new Person("a").Name == "a"? "ok" : "?"));
        Console.WriteLine("ctor " + Try(() => { Object o = new Person(null); return "x"; }));

        // The exception is not made when the value is there.
        Int32 made = 0;
        String chain = Required("x") ?? throw new Exception("never" + (made++));
        Console.WriteLine("chain " + (made == 0? "ok" : "?"));
    }
}
