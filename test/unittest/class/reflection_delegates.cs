// Delegates in reflection: the type of a delegate and its methods, delegates as the params, the results
// and the fields of the methods that Invoke calls (the type is checked), and the methods of a delegate
// that are called through reflection.

/* EXPECT

name Delegate<Int32,Int32>
twice-found True
run 42
make 14
field-get 10
field-set 6
bad-field InvalidCastException
bad-arg InvalidCastException
delegate-invoke 16
same-type True
other-type False
reflect-invoke 42
reflect-two 7
reflect-void 5
reflect-string n7
reflect-bad-arg InvalidCastException
reflect-arg-count ArgumentException

*/

import System;

using System;
using System.Reflection;

class Tool
{
    public Delegate<Int32, Int32> Handler;

    public Tool() { this.Handler = this.Twice; }

    public Int32 Twice(Int32 x) { return x * 2; }

    public Int32 Triple(Int32 x) { return x * 3; }

    public Int32 Run(Delegate<Int32, Int32> f, Int32 v) { return f(v); }

    public Delegate<Int32, Int32> Pick() { return this.Triple; }

    public Int32 Total;

    public Int32 Add(Int32 a, Int32 b) { return a + b; }

    public Void Bump(Int32 n) { this.Total = this.Total + n; }

    public String Name(Int32 n) { return "n" + n; }
}

class Program
{
    static String Kind(Exception e) { return Type.GetType(e).Name; }

    // The method Invoke of the type of a delegate.
    static Method InvokeOf(Type delegateType)
    {
        foreach (Method m in delegateType.GetMethods())
        {
            if (m.Name == "Invoke")
                return m;
        }

        return null;
    }

    [EntryPoint]
    public static void Main()
    {
        Tool tool = new Tool();
        Type type = Type.GetType(tool);

        Delegate<Int32, Int32> twice = tool.Twice;
        Type delegateType = Type.GetType(twice);
        Console.WriteLine("name " + delegateType.Name);

        Method twiceMethod = type.GetMethod("Twice", new Type[] { Type.GetType(1) });
        Console.WriteLine("twice-found " + (twiceMethod != null));

        // A delegate is the argument of a method.
        Object?[] args = new Object?[] { twice, 21 };
        Console.WriteLine("run " + type.GetMethod("Run", new Type[] { delegateType, Type.GetType(1) }).Invoke(tool, args));

        // A delegate is the result, and it is called.
        Delegate<Int32, Int32> made = (Delegate<Int32, Int32>)type.GetMethod("Pick", new Type[0]).Invoke(tool, new Object?[0]);
        Console.WriteLine("make " + (made(2) + made(4) - 4));

        // A delegate is a field.
        Field handler = type.GetField("Handler");
        Delegate<Int32, Int32> got = (Delegate<Int32, Int32>)handler.GetValue(tool);
        Console.WriteLine("field-get " + got(5));

        handler.SetValue(tool, made);
        Delegate<Int32, Int32> now = tool.Handler;
        Console.WriteLine("field-set " + now(2));

        try { handler.SetValue(tool, "bad"); Console.WriteLine("bad-field accepted"); }
        catch (Exception e) { Console.WriteLine("bad-field " + Kind(e)); }

        try
        {
            Object?[] bad = new Object?[] { "bad", 21 };
            type.GetMethod("Run", new Type[] { delegateType, Type.GetType(1) }).Invoke(tool, bad);
            Console.WriteLine("bad-arg accepted");
        }
        catch (Exception e) { Console.WriteLine("bad-arg " + Kind(e)); }

        // A method that is called through the delegate of it.
        Delegate<Int32, Int32> triple = tool.Triple;
        Console.WriteLine("delegate-invoke " + (triple(4) + twice(2)));

        // The type of a delegate is the one of the delegates of its signature.
        Delegate<Int32, Int32> other = tool.Triple;
        Console.WriteLine("same-type " + Type.GetType(other).Equals(delegateType));

        Console.WriteLine("other-type " + delegateType.Equals(typeof(Delegate<Void, Int32>)));

        // The delegate is called through the method Invoke of its type.
        Console.WriteLine("reflect-invoke " + InvokeOf(delegateType).Invoke(twice, new Object?[] { 21 }));

        Delegate<Int32, Int32, Int32> add = tool.Add;
        Console.WriteLine("reflect-two " + InvokeOf(Type.GetType(add)).Invoke(add, new Object?[] { 3, 4 }));

        Delegate<Void, Int32> bump = tool.Bump;
        InvokeOf(Type.GetType(bump)).Invoke(bump, new Object?[] { 5 });
        Console.WriteLine("reflect-void " + tool.Total);

        Delegate<String, Int32> name = tool.Name;
        Console.WriteLine("reflect-string " + InvokeOf(Type.GetType(name)).Invoke(name, new Object?[] { 7 }));

        try { InvokeOf(delegateType).Invoke(twice, new Object?[] { "x" }); Console.WriteLine("reflect-bad-arg accepted"); }
        catch (Exception e) { Console.WriteLine("reflect-bad-arg " + Kind(e)); }

        try { InvokeOf(delegateType).Invoke(twice, new Object?[] { 1, 2 }); Console.WriteLine("reflect-arg-count accepted"); }
        catch (Exception e) { Console.WriteLine("reflect-arg-count " + Kind(e)); }
    }
}
