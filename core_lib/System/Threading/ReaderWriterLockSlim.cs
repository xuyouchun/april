/*
 *  System.dll
 *
 *  ReaderWriterLockSlim, SpinWait and SpinLock.
 */

namespace Threading;

////////// ////////// ////////// ////////// //////////

// A lock that many readers or one writer hold. A writer that waits stops the new readers (the writers do not starve). The lock
// cannot be taken again by the thread that holds it (it would wait for itself): Enter...Lock throws then.
//
// One thread at a time can hold it upgradeable (EnterUpgradeableReadLock): it reads, with the other readers, and can go on to write
// (EnterWriteLock waits for the readers to leave, ExitWriteLock goes back to the upgradeable lock) without a writer or another
// thread that wants to upgrade getting in between: the reads of the thread stay valid until it writes.
public class ReaderWriterLockSlim : IDisposable
{
    private Int32 __readers;
    private Int64 __writer;             // The id of the thread that writes, 0 when there is none.
    private Int64 __upgrader;           // The id of the thread that holds the upgradeable lock, 0 when there is none.
    private Int32 __waitingUpgraders;
    private Int32 __waitingWriters;
    private Int32 __waitingReaders;

    public ReaderWriterLockSlim()
    {
    }

    public Int32 CurrentReadCount => __readers;

    public Boolean IsWriteLockHeld => __writer == Thread.CurrentThread.ManagedThreadId;

    public Boolean IsUpgradeableReadLockHeld => __upgrader == Thread.CurrentThread.ManagedThreadId;

    public Int32 WaitingReadCount => __waitingReaders;

    public Int32 WaitingUpgradeCount => __waitingUpgraders;

    public Int32 WaitingWriteCount => __waitingWriters;

    public Void EnterReadLock()
    {
        TryEnterReadLock(-1);
    }

    // Takes the lock for reading for some milliseconds (-1 is for ever): returns whether it was taken.
    public Boolean TryEnterReadLock(Int32 millisecondsTimeout)
    {
        Waiting.Check(millisecondsTimeout);
        Int64 id = Thread.CurrentThread.ManagedThreadId;

        lock (this)
        {
            if (__writer == id)
                throw new InvalidOperationException("the thread holds the write lock");

            if (__upgrader == id)
                throw new InvalidOperationException("the thread holds the upgradeable read lock");

            Int64 end = Waiting.End(millisecondsTimeout);

            while (__writer != 0 || __waitingWriters > 0)
            {
                Int32 left = Waiting.Left(end, millisecondsTimeout);
                if (left == 0)
                    return false;

                __waitingReaders = __waitingReaders + 1;
                try { Monitor.Wait(this, left); }
                finally { __waitingReaders = __waitingReaders - 1; }
            }

            __readers = __readers + 1;
            return true;
        }
    }

    public Boolean TryEnterReadLock(TimeSpan timeout)
    {
        return TryEnterReadLock(Waiting.ToMilliseconds(timeout));
    }

    public Void ExitReadLock()
    {
        lock (this)
        {
            if (__readers == 0)
                throw new SynchronizationLockException("the read lock is not held");

            __readers = __readers - 1;
            if (__readers == 0)
                Monitor.PulseAll(this);
        }
    }

    public Void EnterWriteLock()
    {
        TryEnterWriteLock(-1);
    }

    // Takes the lock for writing for some milliseconds (-1 is for ever): returns whether it was taken.
    public Boolean TryEnterWriteLock(Int32 millisecondsTimeout)
    {
        Waiting.Check(millisecondsTimeout);
        Int64 id = Thread.CurrentThread.ManagedThreadId;

        lock (this)
        {
            if (__writer == id)
                throw new InvalidOperationException("the thread holds the write lock");

            // The thread that holds the upgradeable lock goes on to write: the others that want to write wait until it is left.
            Boolean upgrading = __upgrader == id;
            Int64 end = Waiting.End(millisecondsTimeout);

            __waitingWriters = __waitingWriters + 1;
            try
            {
                while (__writer != 0 || __readers > 0 || (__upgrader != 0 && !upgrading))
                {
                    Int32 left = Waiting.Left(end, millisecondsTimeout);
                    if (left == 0)
                        return false;

                    Monitor.Wait(this, left);
                }
            }
            finally
            {
                __waitingWriters = __waitingWriters - 1;

                // The readers that the writer stopped can go on when it gives up.
                Monitor.PulseAll(this);
            }

            __writer = id;
            return true;
        }
    }

    public Boolean TryEnterWriteLock(TimeSpan timeout)
    {
        return TryEnterWriteLock(Waiting.ToMilliseconds(timeout));
    }

    public Void ExitWriteLock()
    {
        lock (this)
        {
            if (__writer != Thread.CurrentThread.ManagedThreadId)
                throw new SynchronizationLockException("the write lock is not held by this thread");

            // The thread that went on from the upgradeable lock still holds it.
            __writer = 0;
            Monitor.PulseAll(this);
        }
    }

    public Void EnterUpgradeableReadLock()
    {
        TryEnterUpgradeableReadLock(-1);
    }

    // Takes the lock for reading, with the right to write later, for some milliseconds (-1 is for ever): returns whether it was
    // taken. Only one thread holds it at a time (the readers are not stopped by it).
    public Boolean TryEnterUpgradeableReadLock(Int32 millisecondsTimeout)
    {
        Waiting.Check(millisecondsTimeout);
        Int64 id = Thread.CurrentThread.ManagedThreadId;

        lock (this)
        {
            if (__writer == id)
                throw new InvalidOperationException("the thread holds the write lock");

            if (__upgrader == id)
                throw new InvalidOperationException("the thread holds the upgradeable read lock");

            Int64 end = Waiting.End(millisecondsTimeout);

            while (__writer != 0 || __upgrader != 0 || __waitingWriters > 0)
            {
                Int32 left = Waiting.Left(end, millisecondsTimeout);
                if (left == 0)
                    return false;

                __waitingUpgraders = __waitingUpgraders + 1;
                try { Monitor.Wait(this, left); }
                finally { __waitingUpgraders = __waitingUpgraders - 1; }
            }

            __upgrader = id;
            return true;
        }
    }

    public Boolean TryEnterUpgradeableReadLock(TimeSpan timeout)
    {
        return TryEnterUpgradeableReadLock(Waiting.ToMilliseconds(timeout));
    }

    public Void ExitUpgradeableReadLock()
    {
        lock (this)
        {
            Int64 id = Thread.CurrentThread.ManagedThreadId;

            if (__upgrader != id)
                throw new SynchronizationLockException("the upgradeable read lock is not held by this thread");

            if (__writer == id)
                throw new SynchronizationLockException("the write lock is held: exit it first");

            __upgrader = 0;
            Monitor.PulseAll(this);
        }
    }

    public Void Dispose()
    {
    }
}

////////// ////////// ////////// ////////// //////////

// Waits in a loop: the first times it spins, then it gives the turn to the other threads, then it sleeps a little.
public struct SpinWait
{
    private Int32 __count;

    // How many times SpinOnce was called.
    public Int32 Count => __count;

    // Whether the next SpinOnce gives the turn to the others.
    public Boolean NextSpinWillYield => __count >= 10;

    public Void SpinOnce()
    {
        if (__count < 10)
            Thread.SpinWait(20 << __count);
        else if (__count % 20 == 19)
            Thread.Sleep(1);
        else
            Thread.Yield();

        __count = __count + 1;
    }

    public Void Reset()
    {
        __count = 0;
    }

    // Waits until the condition is true: returns whether it was, before the time (-1 is for ever) was over.
    public static Boolean SpinUntil(Delegate<Boolean> condition, Int32 millisecondsTimeout)
    {
        Waiting.Check(millisecondsTimeout);

        Int64 end = Waiting.End(millisecondsTimeout);
        SpinWait spin = new SpinWait();

        while (!condition())
        {
            if (Waiting.Left(end, millisecondsTimeout) == 0)
                return false;

            spin.SpinOnce();
        }

        return true;
    }

    public static Void SpinUntil(Delegate<Boolean> condition)
    {
        SpinUntil(condition, -1);
    }
}

////////// ////////// ////////// ////////// //////////

// A lock that waits in a loop: for what is held a short time. It does not take itself again (Enter throws).
public struct SpinLock
{
    private Int64 __owner;

    public Boolean IsHeld => __owner != 0;

    public Boolean IsHeldByCurrentThread => __owner == Thread.CurrentThread.ManagedThreadId;

    // Takes the lock, waits until it is free: lockTaken tells it (it is false if Enter throws).
    public Void Enter(ref Boolean lockTaken)
    {
        if (lockTaken)
            throw new ArgumentException("lockTaken must be false");

        Int64 id = Thread.CurrentThread.ManagedThreadId;
        if (__owner == id)
            throw new InvalidOperationException("the thread holds the lock");

        SpinWait spin = new SpinWait();

        for (;;)
        {
            if (Interlocked.CompareExchange(ref __owner, id, 0) == 0)
            {
                lockTaken = true;
                return;
            }

            spin.SpinOnce();
        }
    }

    // Takes the lock if it is free: lockTaken tells it.
    public Void TryEnter(ref Boolean lockTaken)
    {
        if (lockTaken)
            throw new ArgumentException("lockTaken must be false");

        Int64 id = Thread.CurrentThread.ManagedThreadId;

        if (Interlocked.CompareExchange(ref __owner, id, 0) == 0)
            lockTaken = true;
    }

    public Void Exit()
    {
        if (__owner != Thread.CurrentThread.ManagedThreadId)
            throw new SynchronizationLockException("the lock is not held by this thread");

        __owner = 0;
    }
}
