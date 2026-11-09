// Events that have accessors: add and remove keep the handlers where the class wants, raise says
// what is done when the event is raised (and can be public, to raise it from outside); a raise that
// is not written is made for the events that have no accessors. Abstract and virtual events, an
// event of an interface that a class implements, and an explicit implementation.

/* EXPECT

custom add 2
custom raise a1 b1 a2 b2
removed 1
public raise 5 6
base 3
derived 3 mine
interface 9
explicit 1
done

*/

import System;

using System;
using Handler = System.Delegate<void, Int32>;
using Multi = System.MulticastDelegate<void, Int32>;

class Log
{
    // The handlers are kept in a field of the class, and called twice, with the argument and its
    // double, by the raise accessor.
    private Multi __handlers;
    public Int32 Added;

    public event Handler Changed
    {
        add
        {
            __handlers = __handlers + value;
            Added = Added + 1;
        }

        remove
        {
            __handlers = __handlers - value;
        }

        public raise(Int32 x)
        {
            if (__handlers != null)
            {
                __handlers(x);
                __handlers(x * 2);
            }
        }
    }

    public Int32 Count => __handlers == null? 0 : __handlers.Count;
}

class Source
{
    // A raise that is public: the event is raised from outside.
    public event Handler Ready
    {
        add { __list = __list + value; }
        remove { __list = __list - value; }
        public raise(Int32 x) { if (__list != null) __list(x + 1); }
    }

    private Multi __list;
}

abstract class Base
{
    public abstract event Handler Ping;

    public virtual event Handler Pong;

    public void FirePong(Int32 x) { Pong(x); }
}

class Derived : Base
{
    private Multi __ping;
    public String Name = "mine";

    public override event Handler Ping
    {
        add { __ping = __ping + value; }
        remove { __ping = __ping - value; }
        public raise(Int32 x) { if (__ping != null) __ping(x); }
    }

    public void Fire(Int32 x) { Ping(x); }
}

interface IHeart
{
    event Handler Beat;
}

class Heart : IHeart
{
    public event Handler Beat;

    public void Pulse(Int32 x) { Beat(x); }
}

class Explicit : IHeart
{
    private Multi __beats;

    event Handler IHeart.Beat
    {
        add { __beats = __beats + value; }
        remove { __beats = __beats - value; }
        raise(Int32 x) { if (__beats != null) __beats(x); }
    }

    public Int32 Count => __beats == null? 0 : __beats.Count;
}

class Program
{
    static void A(Int32 x) { Console.Write(" a" + x); }
    static void B(Int32 x) { Console.Write(" b" + x); }
    static void Print(Int32 x) { Console.WriteLine("base " + x); }
    static void Pulse(Int32 x) { Console.WriteLine("interface " + x); }
    static void Beat(Int32 x) { Console.WriteLine("explicit " + x); }

    [EntryPoint]
    public static void Main()
    {
        Log log = new Log();
        log.Changed += A;
        log.Changed += B;
        Console.WriteLine("custom add " + log.Added);

        Console.Write("custom raise");
        log.Changed(1);
        Console.WriteLine();

        log.Changed -= A;
        Console.WriteLine("removed " + log.Count);

        Source source = new Source();
        source.Ready += (Int32 x) => { Console.WriteLine("public raise " + x + " " + (x + 1)); };
        source.Ready(4);

        Derived derived = new Derived();
        derived.Pong += Print;
        derived.FirePong(3);
        derived.Ping += (Int32 x) => { Console.WriteLine("derived " + x + " " + derived.Name); };
        derived.Fire(3);

        Heart heart = new Heart();
        IHeart iheart = heart;
        iheart.Beat += Pulse;
        heart.Pulse(9);

        Explicit e = new Explicit();
        IHeart ie = e;
        ie.Beat += Beat;
        Console.WriteLine("explicit " + e.Count);
        Console.WriteLine("done");
    }
}
