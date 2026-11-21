/*
 *  System.dll
 *
 *  ThreadLocal<T>: a value for each thread.
 */

using Collections;

namespace Threading;

////////// ////////// ////////// ////////// //////////

// A value of each thread: a thread that reads it the first time gets the value that the factory makes (or the default of the
// type). The values of the threads that ended stay until Dispose.
public sealed class ThreadLocal<T> : IDisposable
{
    private Dictionary<Int64, T> __values = new Dictionary<Int64, T>();
    private Delegate<T> __factory;
    private Boolean __hasFactory;

    public ThreadLocal()
    {
    }

    public ThreadLocal(Delegate<T> valueFactory)
    {
        __factory = valueFactory;
        __hasFactory = true;
    }

    // The value of the running thread.
    public T Value
    {
        get
        {
            Int64 id = Thread.CurrentThread.ManagedThreadId;
            T value;

            lock (this)
            {
                if (__values.TryGetValue(id, out value))
                    return value;
            }

            // The factory runs out of the lock: it can use other thread locals.
            if (__hasFactory)
                value = __factory();
            else
                value = default(T);

            lock (this)
                __values[id] = value;

            return value;
        }

        set
        {
            Int64 id = Thread.CurrentThread.ManagedThreadId;

            lock (this)
                __values[id] = value;
        }
    }

    // Whether the running thread has a value.
    public Boolean IsValueCreated
    {
        get
        {
            Int64 id = Thread.CurrentThread.ManagedThreadId;
            T value;

            lock (this)
                return __values.TryGetValue(id, out value);
        }
    }

    public Void Dispose()
    {
        lock (this)
            __values = new Dictionary<Int64, T>();
    }
}
