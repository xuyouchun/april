// => throw expression; is the body of a method, a property getter or a lambda that throws.

/* EXPECT

method InvalidOperationException
property InvalidOperationException
lambda InvalidOperationException
void method InvalidOperationException
ok 3

*/

import System;

using System;

class Holder
{
    public Int32 Number => throw new InvalidOperationException("no");

    public Int32 Get() => throw new InvalidOperationException("get");

    public void Run() => throw new InvalidOperationException("run");

    public Int32 Fine() => 3;
}

class Program
{
    static String Name(Action action)
    {
        try
        {
            action();
            return "none";
        }
        catch (Exception e)
        {
            return e.GetType().Name;
        }
    }

    [EntryPoint]
    public static void Main()
    {
        Holder h = new Holder();

        Console.WriteLine("method " + Name(() => { Int32 x = h.Get(); }));
        Console.WriteLine("property " + Name(() => { Int32 x = h.Number; }));

        Func<Int32, Int32> f = x => throw new InvalidOperationException("lambda");
        Console.WriteLine("lambda " + Name(() => { f(1); }));

        Console.WriteLine("void method " + Name(() => h.Run()));
        Console.WriteLine("ok " + h.Fine());
    }
}
