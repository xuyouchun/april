/*
 *  System.dll
 *
 *  ExecutionContext and AsyncLocal<T>: the values that go with the flow of the work (threads that start, work of the pool, timers,
 *  continuations of tasks and of await), not with the thread.
 */

namespace Threading;

////////// ////////// ////////// ////////// //////////

// The values of the AsyncLocal variables that a flow of work has. A context does not change (setting a variable makes another one),
// so the work that was started keeps the values that it was started with, and what it sets is seen only by what it starts later.
public sealed class ExecutionContext
{
    private Object[] __keys;
    private Object?[] __values;

    [ThreadStatic]
    private static ExecutionContext? __current;

    private ExecutionContext(Object[] keys, Object?[] values)
    {
        __keys = keys;
        __values = values;
    }

    // The context of the thread now (null when no AsyncLocal variable has a value).
    public static ExecutionContext? Capture()
    {
        return __current;
    }

    // Runs the method with the context (null: no values), and then goes back to the context that the thread had.
    public static Void Run(ExecutionContext? context, Delegate<Void> callback)
    {
        ExecutionContext? saved = __Swap(context);

        try
        {
            callback();
        }
        finally
        {
            __Swap(saved);
        }
    }

    // Sets the context of the thread, returns the one that it had.
    internal static ExecutionContext? __Swap(ExecutionContext? context)
    {
        ExecutionContext? previous = __current;
        __current = context;

        return previous;
    }

    internal static Object? __Get(Object key)
    {
        ExecutionContext? context = __current;
        if (context == null)
            return null;

        for (Int32 k = 0; k < context.__keys.Length; k++)
        {
            if (Object.ReferenceEquals(context.__keys[k], key))
                return context.__values[k];
        }

        return null;
    }

    // The thread has a new context that has the value (the one it had is not changed: what was started with it keeps its values).
    internal static Void __Set(Object key, Object? value)
    {
        ExecutionContext? context = __current;
        Int32 count = context == null? 0 : context.__keys.Length;
        Int32 index = -1;

        for (Int32 k = 0; k < count; k++)
        {
            if (Object.ReferenceEquals(context.__keys[k], key))
                index = k;
        }

        Int32 size = index >= 0? count : count + 1;
        Object[] keys = new Object[size];
        Object?[] values = new Object?[size];

        for (Int32 k = 0; k < count; k++)
        {
            keys[k] = context.__keys[k];
            values[k] = context.__values[k];
        }

        if (index < 0)
            index = count;

        keys[index] = key;
        values[index] = value;

        __current = new ExecutionContext(keys, values);
    }
}

////////// ////////// ////////// ////////// //////////

// A variable whose value goes with the flow of the work: what a thread, a task, a work item of the pool, a timer or the code after
// an await sees is the value that the code that started it had (a change that it makes is not seen by the code that started it,
// and the ones that were started before it).
public sealed class AsyncLocal<T>
{
    public AsyncLocal()
    {
    }

    public T Value
    {
        get
        {
            Object? value = ExecutionContext.__Get(this);
            if (value == null)
                return default(T);

            return (T)value;
        }

        set
        {
            ExecutionContext.__Set(this, (Object?)value);
        }
    }
}

// Runs a method in a context: the continuations that wait for something (await, ContinueWith, a timer) keep the context of the
// code that asked for them.
internal sealed class ContextCall
{
    private ExecutionContext? __context;
    private Delegate<Void> __method;

    internal ContextCall(ExecutionContext? context, Delegate<Void> method)
    {
        __context = context;
        __method = method;
    }

    internal Void Run()
    {
        ExecutionContext.Run(__context, __method);
    }
}
