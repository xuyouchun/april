// WaitHandle and what is made of it: ManualResetEvent, AutoResetEvent, Semaphore, Mutex, WaitOne with a time, WaitAny, WaitAll.

/* EXPECT

manual 0 2
auto 1 2
semaphore 2
any 1 False False 258
mutex True True err
timeout False

*/

import System;

using System;
using System.Threading;

class Program
{
    static ManualResetEvent __manual = new ManualResetEvent(false);
    static AutoResetEvent __auto = new AutoResetEvent(false);
    static Semaphore __semaphore = new Semaphore(2, 2);
    static Int32 __woken;
    static Int32 __inside;
    static Int32 __maxInside;

    static Void WaitManual() { __manual.WaitOne(); Interlocked.Increment(ref __woken); }

    static Void WaitAuto() { __auto.WaitOne(); Interlocked.Increment(ref __woken); }

    static Void UseSemaphore()
    {
        __semaphore.WaitOne();

        Int32 n = Interlocked.Increment(ref __inside);
        lock (typeof(Program))
        {
            if (n > __maxInside)
                __maxInside = n;
        }

        Thread.Sleep(30);
        Interlocked.Decrement(ref __inside);
        __semaphore.Release();
    }

    [EntryPoint]
    public static void Main()
    {
        Thread a = new Thread(WaitManual), b = new Thread(WaitManual);
        a.Start(); b.Start();
        Thread.Sleep(40);
        Int32 before = __woken;
        __manual.Set();
        a.Join(); b.Join();
        Console.WriteLine("manual " + before + " " + __woken);

        __woken = 0;
        Thread c = new Thread(WaitAuto), d = new Thread(WaitAuto);
        c.Start(); d.Start();
        Thread.Sleep(40);
        __auto.Set();
        Thread.Sleep(80);
        Int32 one = __woken;
        __auto.Set();
        c.Join(); d.Join();
        Console.WriteLine("auto " + one + " " + __woken);

        Thread[] users = new Thread[5];
        for (Int32 i = 0; i < 5; i++)
        {
            users[i] = new Thread(UseSemaphore);
            users[i].Start();
        }

        for (Int32 i = 0; i < 5; i++)
            users[i].Join();

        Console.WriteLine("semaphore " + __maxInside);

        WaitHandle[] handles = new WaitHandle[2];
        handles[0] = new ManualResetEvent(false);
        handles[1] = new ManualResetEvent(true);
        Int32 any = WaitHandle.WaitAny(handles, 10);
        Boolean all = WaitHandle.WaitAll(handles, 10);
        Boolean first = handles[0].WaitOne(10);
        ((ManualResetEvent)handles[0]).Set();
        Boolean all2 = WaitHandle.WaitAll(handles, 10);
        Console.WriteLine("any " + any + " " + all + " " + first + " " + WaitHandle.WaitTimeout);

        Mutex m = new Mutex();
        Boolean t1 = m.WaitOne(), t2 = m.WaitOne();
        m.ReleaseMutex(); m.ReleaseMutex();
        String error = "";
        try { m.ReleaseMutex(); } catch (SynchronizationLockException e) { error = "err"; }
        Console.WriteLine("mutex " + t1 + " " + t2 + " " + error);

        Console.WriteLine("timeout " + new AutoResetEvent(false).WaitOne(20));
    }
}
