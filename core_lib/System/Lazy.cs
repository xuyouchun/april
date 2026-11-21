/*
 *  System.dll
 *
 *  Lazy<T>: a value that is made when it is first read.
 */

////////// ////////// ////////// ////////// //////////

// A value that a factory makes the first time that it is read; with many threads, the factory runs once (the others wait
// for the value). An exception of the factory is thrown to the reader, the value is not made then (the next read tries again).
public sealed class Lazy<T>
{
    private Delegate<T> __factory;
    private T __value;
    private Boolean __created;
    private Boolean __threadSafe;

    public Lazy(Delegate<T> valueFactory)
    {
        __factory = valueFactory;
        __threadSafe = true;
    }

    // `isThreadSafe` is false: the value is made without a lock, for a value that one thread reads.
    public Lazy(Delegate<T> valueFactory, Boolean isThreadSafe)
    {
        __factory = valueFactory;
        __threadSafe = isThreadSafe;
    }

    public Boolean IsValueCreated => __created;

    public T Value
    {
        get
        {
            if (__created)
                return __value;

            if (!__threadSafe)
            {
                __value = __factory();
                __created = true;

                return __value;
            }

            lock (this)
            {
                if (!__created)
                {
                    __value = __factory();
                    __created = true;
                }

                return __value;
            }
        }
    }
}
