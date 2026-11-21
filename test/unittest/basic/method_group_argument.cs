// A method group as the argument of a constructor or of a call: the delegate type of the parameter tells the method, also
// when the constructors are overloaded (the type that the result is assigned to is not the context of the argument).

/* EXPECT

a b vb
call 1
call 2
thread 5

*/

import System;

using System;
using System.Threading;

class W
{
    public String Kind;

    public W(Delegate<Void> start) { Kind = "a"; }

    public W(Delegate<Void, Object> start) { Kind = "b"; }
}

class V
{
    public String Kind;

    public V(Delegate<Void, Object> start) { Kind = "vb"; }
}

class Program
{
    static Int32 __total;

    static Void Run() { }

    static Void Run1(Object o) { }

    static Void Call(Delegate<Void> d) { d(); }

    static Void Call(Delegate<Void, Object> d) { d(2); }

    static Void Show() { Console.WriteLine("call 1"); }

    static Void Show2(Object o) { Console.WriteLine("call " + o); }

    static Void Add(Object o) { __total = __total + (Int32)o; }

    [EntryPoint]
    public static void Main()
    {
        W w1 = new W(Run), w2 = new W(Run1);
        V v = new V(Run1);
        Console.WriteLine(w1.Kind + " " + w2.Kind + " " + v.Kind);

        Call(Show);
        Call(Show2);

        Thread t = new Thread(Add);
        t.Start(5);
        t.Join();
        Console.WriteLine("thread " + __total);
    }
}
