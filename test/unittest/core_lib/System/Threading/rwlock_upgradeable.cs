// ReaderWriterLockSlim: the upgradeable read lock (one thread at a time, with the readers, can go on to write).

/* EXPECT

held True False
readers with upgradeable 2 True
second upgradeable timed out True
writer waits for upgradeable True
upgrade waits readers True
back to upgradeable True False True
nobody in between 10 True
errors InvalidOperationException InvalidOperationException SynchronizationLockException SynchronizationLockException
done

*/

import System;

using System;
using System.Threading;

class Program
{
    static ReaderWriterLockSlim __lock = new ReaderWriterLockSlim();
    static Int32 __value;
    static Boolean __writerGotIt;
    static Boolean __readerIn;
    static Boolean __upgraded;
    static Int32 __bumps;

    static Void ReadHold()
    {
        __lock.EnterReadLock();
        __readerIn = true;
        Thread.Sleep(120);
        __lock.ExitReadLock();
    }

    static Void Writer()
    {
        __lock.EnterWriteLock();
        __writerGotIt = true;
        __lock.ExitWriteLock();
    }

    static Void Upgrader()
    {
        __lock.EnterUpgradeableReadLock();
        __lock.EnterWriteLock();
        __upgraded = true;
        __lock.ExitWriteLock();
        __lock.ExitUpgradeableReadLock();
    }

    // Reads the value, then writes it +1 : the value read is still the one when it writes.
    static Void Bump()
    {
        for (Int32 i = 0; i < 5; i++)
        {
            __lock.EnterUpgradeableReadLock();
            Int32 seen = __value;
            Thread.Sleep(1);
            __lock.EnterWriteLock();

            if (__value == seen)
                __bumps++;

            __value = seen + 1;
            __lock.ExitWriteLock();
            __lock.ExitUpgradeableReadLock();
        }
    }

    [EntryPoint]
    public static void Main()
    {
        __lock.EnterUpgradeableReadLock();
        Console.WriteLine("held " + __lock.IsUpgradeableReadLockHeld + " " + __lock.IsWriteLockHeld);
        __lock.ExitUpgradeableReadLock();

        // The readers are not stopped by the thread that holds the upgradeable lock.
        __lock.EnterUpgradeableReadLock();
        Thread reader = new Thread(ReadHold);
        reader.Start();
        while (!__readerIn)
            Thread.Sleep(5);

        Console.WriteLine("readers with upgradeable " + (__lock.CurrentReadCount + 1) + " " + __lock.IsUpgradeableReadLockHeld);

        // Only one thread holds it.
        Boolean[] second = new Boolean[1];
        Thread other = new Thread(() => { second[0] = __lock.TryEnterUpgradeableReadLock(30); });
        other.Start();
        other.Join();
        Console.WriteLine("second upgradeable timed out " + !second[0]);

        // A writer waits until the upgradeable lock is left.
        Thread writer = new Thread(Writer);
        writer.Start();
        Thread.Sleep(60);
        Boolean waited = !__writerGotIt;
        reader.Join();
        __lock.ExitUpgradeableReadLock();
        writer.Join();
        Console.WriteLine("writer waits for upgradeable " + (waited && __writerGotIt));

        // The upgrade waits for the readers to leave.
        __readerIn = false;
        reader = new Thread(ReadHold);
        reader.Start();
        while (!__readerIn)
            Thread.Sleep(5);

        Thread upgrader = new Thread(Upgrader);
        upgrader.Start();
        Thread.Sleep(30);
        Boolean early = __upgraded;
        upgrader.Join();
        reader.Join();
        Console.WriteLine("upgrade waits readers " + (!early && __upgraded));

        // ExitWriteLock goes back to the upgradeable lock.
        __lock.EnterUpgradeableReadLock();
        __lock.EnterWriteLock();
        Boolean writing = __lock.IsWriteLockHeld;
        __lock.ExitWriteLock();
        Console.WriteLine("back to upgradeable " + __lock.IsUpgradeableReadLockHeld + " " + __lock.IsWriteLockHeld + " " + writing);
        __lock.ExitUpgradeableReadLock();

        // Two threads that read and then write do not lose an update.
        Thread t1 = new Thread(Bump);
        Thread t2 = new Thread(Bump);
        t1.Start();
        t2.Start();
        t1.Join();
        t2.Join();
        Console.WriteLine("nobody in between " + __value + " " + (__bumps == 10));

        // The errors.
        String e1 = "", e2 = "", e3 = "", e4 = "";

        __lock.EnterUpgradeableReadLock();
        try { __lock.EnterReadLock(); } catch (Exception e) { e1 = e.GetType().Name; }
        try { __lock.EnterUpgradeableReadLock(); } catch (Exception e) { e2 = e.GetType().Name; }
        __lock.EnterWriteLock();
        try { __lock.ExitUpgradeableReadLock(); } catch (Exception e) { e3 = e.GetType().Name; }
        __lock.ExitWriteLock();
        __lock.ExitUpgradeableReadLock();
        try { __lock.ExitUpgradeableReadLock(); } catch (Exception e) { e4 = e.GetType().Name; }

        Console.WriteLine("errors " + e1 + " " + e2 + " " + e3 + " " + e4);
        Console.WriteLine("done");
    }
}
