// System.GC, WeakReference and the destructors. The collector looks at the stacks without knowing what a word is, so an
// object may stay alive because of a stale word: the checks leave a margin (a few objects, a few rounds) where it is so.

/* EXPECT

count True
finalized True
suppressed 0
chain True
weak True
weak kept True True
generic True True
memory True True
generation 0 0

*/

import System;

using System;

class Res
{
    public static Int32 Count;
    public Int32 Id;
    public Res(Int32 id) { Id = id; }
    public ~Res() { Count++; }
}

class Quiet
{
    public static Int32 Count;
    public ~Quiet() { Count++; }
}

class Base
{
    public static String Log = "";
    public ~Base() { Log += "B"; }
}

class Derived : Base
{
    public ~Derived() { Log += "D"; }
}

class Item
{
    public Int32 V;
}

class Program
{
    static Int32 Make(Int32 n)
    {
        Int32 sum = 0;
        for (Int32 i = 0; i < n; i++)
        {
            Res r = new Res(i);
            sum += r.Id;
        }

        return sum;
    }

    static Int32 MakeQuiet(Int32 n)
    {
        for (Int32 i = 0; i < n; i++)
        {
            Quiet q = new Quiet();
            GC.SuppressFinalize(q);
        }

        return n;
    }

    static Int32 MakeDerived(Int32 n)
    {
        for (Int32 i = 0; i < n; i++)
        {
            Derived d = new Derived();
        }

        return n;
    }

    static WeakReference Weak(Int32 v)
    {
        Item o = new Item();
        o.V = v;
        return new WeakReference(o);
    }

    static Boolean ChainIsRight()
    {
        String log = Base.Log;
        if (log.Length < 8 || log.Length % 2 != 0)
            return false;

        for (Int32 k = 0; k < log.Length; k += 2)
        {
            if (log[k] != 'D' || log[k + 1] != 'B')
                return false;
        }

        return true;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32 c0 = GC.CollectionCount();
        GC.Collect();
        Console.WriteLine("count " + (GC.CollectionCount() > c0));

        // The destructors of the objects that nothing reaches run, at a call of the program after the collection.
        Make(10);
        for (Int32 round = 0; round < 6 && Res.Count < 8; round++)
        {
            GC.Collect();
            GC.WaitForPendingFinalizers();
        }

        Console.WriteLine("finalized " + (Res.Count >= 8 && Res.Count <= 10));

        // The destructor of an object that was suppressed does not run.
        MakeQuiet(10);
        for (Int32 round = 0; round < 3; round++)
        {
            GC.Collect();
            GC.WaitForPendingFinalizers();
        }

        Console.WriteLine("suppressed " + Quiet.Count);

        // The destructor of the class runs, then the one of its base class.
        MakeDerived(6);
        for (Int32 round = 0; round < 6 && Base.Log.Length < 10; round++)
        {
            GC.Collect();
            GC.WaitForPendingFinalizers();
        }

        Console.WriteLine("chain " + ChainIsRight());

        // A weak reference does not keep the object alive.
        WeakReference[] refs = new WeakReference[20];
        for (Int32 i = 0; i < 20; i++)
            refs[i] = Weak(i);

        GC.Collect();
        Int32 alive = 0;
        for (Int32 i = 0; i < 20; i++)
        {
            if (refs[i].IsAlive)
                alive++;
        }

        Console.WriteLine("weak " + (alive <= 3));

        // ... and gives it while something else does.
        Item keep = new Item();
        keep.V = 7;
        WeakReference w = new WeakReference(keep);
        GC.Collect();
        Console.WriteLine("weak kept " + w.IsAlive + " " + Object.ReferenceEquals(w.Target, keep));

        WeakReference<Item> generic = new WeakReference<Item>(keep);
        Item got;
        Boolean found = generic.TryGetTarget(out got);
        GC.KeepAlive(keep);
        Console.WriteLine("generic " + found + " " + (found && got.V == 7));

        // The memory in use.
        Int64 m0 = GC.GetTotalMemory(true);
        Int32[] big = new Int32[1000000];
        big[5] = 1;
        Int64 m1 = GC.GetTotalMemory(false);
        big = null;
        Int64 m2 = GC.GetTotalMemory(true);
        Console.WriteLine("memory " + (m1 - m0 > 3000000) + " " + (m1 - m2 > 3000000));

        Console.WriteLine("generation " + GC.MaxGeneration + " " + GC.GetGeneration(keep));
    }
}
