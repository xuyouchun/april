/*
 *  System.dll
 *
 *  ThreadPool: threads that run the work that is queued.
 */

using Collections;

namespace Threading;

////////// ////////// ////////// ////////// //////////

// A piece of work of the pool: a method with a state, or a method without one.
internal sealed class WorkItem
{
    internal Delegate<Void, Object> WithState;
    internal Delegate<Void> Plain;
    internal Boolean HasState;
    internal Object? State;
    internal ExecutionContext? Context = ExecutionContext.Capture();

    internal Void Run()
    {
        ExecutionContext? saved = ExecutionContext.__Swap(Context);

        try
        {
            if (HasState)
                WithState(State);
            else
                Plain();
        }
        finally
        {
            ExecutionContext.__Swap(saved);
        }
    }
}

// The pool: a queue of work and the background threads that take it. A thread is made when there is more work than idle
// threads (up to the maximum: the work can wait for other work, so the pool grows rather than blocks), and ends when it has
// been idle for a while. An exception that nothing catches in the work ends the program, as in any thread.
public static class ThreadPool
{
    private static Object __gate = new Object();
    private static Queue<WorkItem> __queue = new Queue<WorkItem>();
    private static Int32 __workers;
    private static Int32 __idle;
    private static Int32 __maxThreads = 64;
    private static Int32 __completed;

    // The idle time after which a thread of the pool ends, in milliseconds.
    private const Int32 __IdleTime = 3000;

    // Queues a method with a state, returns true.
    public static Boolean QueueUserWorkItem(Delegate<Void, Object> callBack, Object? state)
    {
        WorkItem item = new WorkItem();
        item.WithState = callBack;
        item.HasState = true;
        item.State = state;

        __Enqueue(item);
        return true;
    }

    public static Boolean QueueUserWorkItem(Delegate<Void, Object> callBack)
    {
        return QueueUserWorkItem(callBack, null);
    }

    // Queues a method without a state.
    public static Boolean QueueUserWorkItem(Delegate<Void> callBack)
    {
        WorkItem item = new WorkItem();
        item.Plain = callBack;

        __Enqueue(item);
        return true;
    }

    // The threads of the pool that exist.
    public static Int32 ThreadCount
    {
        get
        {
            lock (__gate)
                return __workers;
        }
    }

    // The work that waits for a thread.
    public static Int32 PendingWorkItemCount
    {
        get
        {
            lock (__gate)
                return __queue.Count;
        }
    }

    // The work that was run.
    public static Int32 CompletedWorkItemCount
    {
        get
        {
            lock (__gate)
                return __completed;
        }
    }

    // Sets the number of threads that the pool can make.
    public static Boolean SetMaxThreads(Int32 workerThreads)
    {
        if (workerThreads < 1)
            return false;

        lock (__gate)
            __maxThreads = workerThreads;

        return true;
    }

    private static Void __Enqueue(WorkItem item)
    {
        Boolean start = false;

        lock (__gate)
        {
            __queue.Enqueue(item);

            // More work than threads that are idle (the ones that were woken up and did not run yet count as idle).
            if (__queue.Count > __idle && __workers < __maxThreads)
            {
                __workers++;
                start = true;
            }

            Monitor.Pulse(__gate);
        }

        if (start)
        {
            Thread thread = new Thread(__Work);
            thread.IsBackground = true;
            thread.Start();
        }
    }

    // The thread of the pool.
    private static Void __Work()
    {
        for (;;)
        {
            WorkItem? item = null;

            lock (__gate)
            {
                while (__queue.Count == 0)
                {
                    __idle++;
                    Boolean woken = Monitor.Wait(__gate, __IdleTime);
                    __idle--;

                    if (!woken && __queue.Count == 0)
                    {
                        __workers--;
                        return;
                    }
                }

                item = __queue.Dequeue();
            }

            item.Run();

            lock (__gate)
                __completed++;
        }
    }
}
