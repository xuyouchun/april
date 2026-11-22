/*
 *  System.dll
 *
 *  Thread: a thread of the program.
 */

namespace Threading;

////////// ////////// ////////// ////////// //////////

// A thread of the program. The threads are real, but one of them runs at a time: it gives the others their turn when it
// waits (Sleep, Join, Monitor.Wait, a lock that is held by another thread), and after a short time when it does not wait.
// A thread is made with the method that it runs, a Delegate<Void> (ThreadStart) or a Delegate<Void, Object>
// (ParameterizedThreadStart), and runs when Start is called. An exception that nothing catches in a thread ends the program.
public sealed class Thread
{
    private Delegate<Void> __start;
    private Delegate<Void, Object> __parameterized;
    private Boolean __hasParameter;
    private Object? __parameter;
    private Int64 __id;
    private Boolean __started;
    private Boolean __background;
    private String? __name;

    private static Thread? __main;

    // A thread that runs a method that has no parameter.
    public Thread(Delegate<Void> start)
    {
        __start = start;
    }

    // A thread that runs a method that has a parameter: Start(parameter).
    public Thread(Delegate<Void, Object> start)
    {
        __parameterized = start;
        __hasParameter = true;
    }

    // The thread that started the program.
    private Thread(Int64 id)
    {
        __id = id;
        __started = true;
    }

    // The thread that is running.
    public static Thread CurrentThread
    {
        get
        {
            Object? current = Internal.Thread_GetCurrent();
            if (current != null)
                return (Thread)current;

            if (__main == null)
                __main = new Thread(Internal.Thread_CurrentId());

            return __main;
        }
    }

    // The number of the thread: 1 is the thread that started the program.
    public Int32 ManagedThreadId => (Int32)__id;

    // The name of the thread, for the one that reads it.
    public String? Name
    {
        get => __name;
        set { __name = value; }
    }

    // Whether the thread has started and has not ended.
    public Boolean IsAlive => __started && Internal.Thread_IsAlive(__id);

    // Whether the program does not wait for the thread: it ends when the threads that are not in the background end.
    // It can be changed before and after the thread starts.
    public Boolean IsBackground
    {
        get => __started? Internal.Thread_IsBackground(__id) : __background;

        set
        {
            __background = value;

            if (__started)
                Internal.Thread_SetBackground(__id, value);
        }
    }

    // Starts the thread: it runs the method of the thread with no parameter (null for a method that has one).
    public Void Start()
    {
        __Start(null);
    }

    // Starts the thread with the parameter of its method.
    public Void Start(Object? parameter)
    {
        if (!__hasParameter)
            throw new InvalidOperationException("the method of the thread has no parameter");

        __Start(parameter);
    }

    // Waits for the end of the thread.
    public Void Join()
    {
        Join(-1);
    }

    // Waits for the end of the thread for some milliseconds (-1 is for ever), returns whether it ended.
    public Boolean Join(Int32 millisecondsTimeout)
    {
        if (!__started)
            throw new InvalidOperationException("the thread has not started");

        if (millisecondsTimeout < -1)
            throw new ArgumentException("the time is negative");

        if (__id == CurrentThread.__id)
            throw new InvalidOperationException("a thread cannot wait for itself");

        return Internal.Thread_Join(__id, millisecondsTimeout);
    }

    public Boolean Join(TimeSpan timeout) => Join(__ToMilliseconds(timeout));

    // The thread that is running waits for some milliseconds: the other threads run.
    public static Void Sleep(Int32 millisecondsTimeout)
    {
        if (millisecondsTimeout < -1)
            throw new ArgumentException("the time is negative");

        Internal.Thread_Sleep(millisecondsTimeout);
    }

    public static Void Sleep(TimeSpan timeout)
    {
        Internal.Thread_Sleep(__ToMilliseconds(timeout));
    }

    // The thread that is running gives its turn to the others.
    public static Void Yield()
    {
        Internal.Thread_Yield();
    }

    // Whether the threads of the program run in parallel (the native programs), or one at a time.
    public static Boolean IsParallel => Internal.Thread_IsParallel();

    // The thread that is running waits in a loop of some iterations (the other threads can take their turn in the loop).
    public static Void SpinWait(Int32 iterations)
    {
        for (Int32 k = 0; k < iterations; k++)
        {
        }
    }

    // A barrier of the memory: the threads see the operations in order, nothing to do.
    public static Void MemoryBarrier()
    {
    }

    private Void __Start(Object? parameter)
    {
        if (__started)
            throw new InvalidOperationException("the thread has started");

        __parameter = parameter;
        __started = true;
        __id = Internal.Thread_Start(this, __background);
    }

    // The method that the thread runs, called by the runtime.
    private static Void __Entry(Thread thread)
    {
        thread.__Run();
    }

    private Void __Run()
    {
        if (__hasParameter)
            __parameterized(__parameter);
        else
            __start();
    }

    private static Int32 __ToMilliseconds(TimeSpan timeout)
    {
        Double milliseconds = timeout.TotalMilliseconds;

        if (milliseconds < -1.0 || milliseconds > 2147483647.0)
            throw new ArgumentException("the time is out of range");

        return (Int32)milliseconds;
    }
}
