/*
 *  System.dll
 *
 *  BlockingCollection<T>: a queue where Take waits for an item, and Add waits for room when it has a limit.
 */

using Threading;

namespace Collections.Concurrent;

////////// ////////// ////////// ////////// //////////

// Producers add, consumers take; CompleteAdding tells that no item will be added: the consumers go on until the queue is empty,
// then Take throws and IsCompleted is true.
public class BlockingCollection<T> : IDisposable
{
    private Queue<T> __items = new Queue<T>();
    private Int32 __bound;                  // 0 for no limit.
    private Boolean __addingCompleted;

    public BlockingCollection()
    {
    }

    // A collection that waits in Add when it has `boundedCapacity` items.
    public BlockingCollection(Int32 boundedCapacity)
    {
        if (boundedCapacity < 1)
            throw new ArgumentException("the capacity is not positive");

        __bound = boundedCapacity;
    }

    public Int32 Count
    {
        get
        {
            lock (this)
                return __items.Count;
        }
    }

    public Int32 BoundedCapacity => __bound == 0? -1 : __bound;

    public Boolean IsAddingCompleted => __addingCompleted;

    // No item will be added and none is left.
    public Boolean IsCompleted
    {
        get
        {
            lock (this)
                return __addingCompleted && __items.Count == 0;
        }
    }

    public Void Add(T item)
    {
        TryAdd(item, -1);
    }

    // Adds the item, waits for room for some milliseconds (-1 is for ever): returns whether it was added.
    public Boolean TryAdd(T item, Int32 millisecondsTimeout)
    {
        Waiting.Check(millisecondsTimeout);

        lock (this)
        {
            Int64 end = Waiting.End(millisecondsTimeout);

            for (;;)
            {
                if (__addingCompleted)
                    throw new InvalidOperationException("the collection does not accept items any more");

                if (__bound == 0 || __items.Count < __bound)
                    break;

                Int32 left = Waiting.Left(end, millisecondsTimeout);
                if (left == 0)
                    return false;

                Monitor.Wait(this, left);
            }

            __items.Enqueue(item);
            Monitor.PulseAll(this);

            return true;
        }
    }

    // Takes the first item, waits for one: throws InvalidOperationException when no item will come.
    public T Take()
    {
        T item;

        if (!TryTake(out item, -1))
            throw new InvalidOperationException("the collection is completed");

        return item;
    }

    // Takes the first item, waits for some milliseconds (-1 is for ever): returns false when there is none (the time is over,
    // or no item will come).
    public Boolean TryTake(out T item, Int32 millisecondsTimeout)
    {
        Waiting.Check(millisecondsTimeout);

        lock (this)
        {
            Int64 end = Waiting.End(millisecondsTimeout);

            while (__items.Count == 0)
            {
                if (__addingCompleted)
                {
                    item = default(T);
                    return false;
                }

                Int32 left = Waiting.Left(end, millisecondsTimeout);
                if (left == 0)
                {
                    item = default(T);
                    return false;
                }

                Monitor.Wait(this, left);
            }

            item = __items.Dequeue();
            Monitor.PulseAll(this);

            return true;
        }
    }

    public Boolean TryTake(out T item)
    {
        return TryTake(out item, 0);
    }

    // The collection does not accept items any more; the threads that wait go on.
    public Void CompleteAdding()
    {
        lock (this)
        {
            __addingCompleted = true;
            Monitor.PulseAll(this);
        }
    }

    // The items, taken one by one (it waits for each) until the collection is completed.
    public IEnumerable<T> GetConsumingEnumerable()
    {
        for (;;)
        {
            T item;
            if (!TryTake(out item, -1))
                yield break;

            yield return item;
        }
    }

    public Void Dispose()
    {
    }
}
