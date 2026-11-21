// SemaphoreSlim, ManualResetEventSlim, CountdownEvent, Barrier (phases and the action), ReaderWriterLockSlim, SpinLock, SpinWait.

/* EXPECT

semaphore 3 True False
event False True 2
countdown 0 True 3
barrier 3 6 phases 3
rwlock readers 3 writer True 0
spin 2000 True
spinwait True

*/

import System;

using System;
using System.Threading;

class Program
{
    static SemaphoreSlim __semaphore = new SemaphoreSlim(3);
    static ManualResetEventSlim __event = new ManualResetEventSlim(false);
    static CountdownEvent __countdown = new CountdownEvent(3);
    static Barrier? __barrier;
    static ReaderWriterLockSlim __lock = new ReaderWriterLockSlim();
    static SpinLock __spin = new SpinLock();
    static Int32 __shared;
    static Int32 __phasesRun;
    static Int32 __readers;
    static Int32 __maxReaders;
    static Int32 __done;

    static Void Signal() { Thread.Sleep(20); __countdown.Signal(); }

    static Void WaitEvent() { __event.Wait(); Interlocked.Increment(ref __done); }

    static Void Phases()
    {
        for (Int32 i = 0; i < 3; i++)
            __barrier.SignalAndWait();
    }

    static Void AfterPhase(Barrier b) { __phasesRun++; }

    static Void Read()
    {
        __lock.EnterReadLock();

        Int32 n = Interlocked.Increment(ref __readers);
        lock (typeof(Program))
        {
            if (n > __maxReaders)
                __maxReaders = n;
        }

        Thread.Sleep(40);
        Interlocked.Decrement(ref __readers);
        __lock.ExitReadLock();
    }

    static Void Spin()
    {
        for (Int32 i = 0; i < 500; i++)
        {
            Boolean taken = false;
            __spin.Enter(ref taken);
            try { __shared = __shared + 1; Thread.Yield(); }
            finally { if (taken) __spin.Exit(); }
        }
    }

    [EntryPoint]
    public static void Main()
    {
        Boolean w1 = __semaphore.Wait(10);
        __semaphore.Wait(); __semaphore.Wait();
        Boolean w2 = __semaphore.Wait(10);
        __semaphore.Release(3);
        Console.WriteLine("semaphore " + __semaphore.CurrentCount + " " + w1 + " " + w2);

        Thread e1 = new Thread(WaitEvent), e2 = new Thread(WaitEvent);
        e1.Start(); e2.Start();
        Thread.Sleep(30);
        Boolean before = __event.Wait(5);
        __event.Set();
        e1.Join(); e2.Join();
        Console.WriteLine("event " + (__done == 0) + " " + __event.IsSet + " " + __done);

        Thread[] signalers = new Thread[3];
        for (Int32 i = 0; i < 3; i++)
        {
            signalers[i] = new Thread(Signal);
            signalers[i].Start();
        }

        Boolean reached = __countdown.Wait(5000);
        Console.WriteLine("countdown " + __countdown.CurrentCount + " " + reached + " " + __countdown.InitialCount);

        Delegate<Void, Barrier> after = AfterPhase;
        __barrier = new Barrier(3, after);
        Thread[] members = new Thread[3];
        for (Int32 i = 0; i < 3; i++)
        {
            members[i] = new Thread(Phases);
            members[i].Start();
        }

        for (Int32 i = 0; i < 3; i++)
            members[i].Join();

        Console.WriteLine("barrier " + __barrier.ParticipantCount + " " + (__phasesRun * 2) + " phases " + __barrier.CurrentPhaseNumber);

        Thread[] readers = new Thread[3];
        for (Int32 i = 0; i < 3; i++)
        {
            readers[i] = new Thread(Read);
            readers[i].Start();
        }

        for (Int32 i = 0; i < 3; i++)
            readers[i].Join();

        __lock.EnterWriteLock();
        Boolean held = __lock.IsWriteLockHeld;
        __lock.ExitWriteLock();
        Console.WriteLine("rwlock readers " + __maxReaders + " writer " + held + " " + __lock.CurrentReadCount);

        Thread[] spinners = new Thread[4];
        for (Int32 i = 0; i < 4; i++)
        {
            spinners[i] = new Thread(Spin);
            spinners[i].Start();
        }

        for (Int32 i = 0; i < 4; i++)
            spinners[i].Join();

        Console.WriteLine("spin " + __shared + " " + !__spin.IsHeld);

        SpinWait wait = new SpinWait();
        for (Int32 i = 0; i < 12; i++)
            wait.SpinOnce();

        Console.WriteLine("spinwait " + (wait.Count == 12 && wait.NextSpinWillYield));
    }
}
