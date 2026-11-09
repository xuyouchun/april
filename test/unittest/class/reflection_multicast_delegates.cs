// The handlers of an event (a MulticastDelegate) and the delegates that take or return structs, called
// through the methods of their types.

/* EXPECT

total 15
count 2
reflect-invoke 45
struct-arg 7
struct-mixed 2431
struct-result 6 12

*/

import System;

using System;
using System.Reflection;

struct P
{
    public Int32 X;
    public Int32 Y;
}

class Source
{
    public event Delegate<Void, Int32> Changed;
    public Int32 Total;

    public Void Add(Int32 n) { this.Total = this.Total + n; }

    public Void Twice(Int32 n) { this.Total = this.Total + n * 2; }

    public Void Raise(Int32 n) { if (Changed != null) Changed(n); }

    public Int32 Sum(P p) { return p.X + p.Y; }

    public P Make(Int32 n) { P p = new P(); p.X = n; p.Y = n * 2; return p; }

    public Int32 Mix(Int32 a, P p, Int32 b) { return a + p.X * 10 + p.Y * 100 + b * 1000; }
}

class Program
{
    static Method MethodOf(Type type, String name)
    {
        foreach (Method m in type.GetMethods())
        {
            if (m.Name == name)
                return m;
        }

        return null;
    }

    [EntryPoint]
    public static void Main()
    {
        Source source = new Source();
        source.Changed += source.Add;
        source.Changed += source.Twice;
        source.Raise(5);
        Console.WriteLine("total " + source.Total);

        // The handlers are the field of the event: a MulticastDelegate.
        Object handlers = null;
        foreach (Field f in Type.GetType(source).GetFields())
        {
            String name = f.FieldType.Name;
            if (name.Length > 9 && name.Substring(0, 9) == "Multicast")
                handlers = f.GetValue(source);
        }

        Type handlersType = Type.GetType(handlers);
        Console.WriteLine("count " + MethodOf(handlersType, "get_Count").Invoke(handlers, new Object?[0]));

        MethodOf(handlersType, "Invoke").Invoke(handlers, new Object?[] { 10 });
        Console.WriteLine("reflect-invoke " + source.Total);

        // Structs are passed and returned by value.
        Delegate<Int32, P> sum = source.Sum;
        P p = new P();
        p.X = 3;
        p.Y = 4;
        Console.WriteLine("struct-arg " + MethodOf(Type.GetType(sum), "Invoke").Invoke(sum, new Object?[] { p }));

        Delegate<Int32, Int32, P, Int32> mix = source.Mix;
        Console.WriteLine("struct-mixed " + MethodOf(Type.GetType(mix), "Invoke").Invoke(mix, new Object?[] { 1, p, 2 }));

        Delegate<P, Int32> make = source.Make;
        Object made = MethodOf(Type.GetType(make), "Invoke").Invoke(make, new Object?[] { 6 });
        P q = (P)made;
        Console.WriteLine("struct-result " + q.X + " " + q.Y);
    }
}
