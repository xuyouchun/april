/*
 *  System.dll
 *
 *  ConcurrentQueue<T>, ConcurrentStack<T>, ConcurrentBag<T>: collections that many threads use; each call holds the lock of the
 *  collection.
 */

using Threading;

namespace Collections.Concurrent;

////////// ////////// ////////// ////////// //////////

// A queue: first in, first out.
public class ConcurrentQueue<T> : IReadOnlyCollection<T>
{
    private Queue<T> __items = new Queue<T>();

    public ConcurrentQueue()
    {
    }

    public Int32 Count
    {
        get
        {
            lock (this)
                return __items.Count;
        }
    }

    public Boolean IsEmpty => Count == 0;

    public Void Enqueue(T item)
    {
        lock (this)
            __items.Enqueue(item);
    }

    // Takes the first item: returns false when there is none.
    public Boolean TryDequeue(out T result)
    {
        lock (this)
        {
            if (__items.Count == 0)
            {
                result = default(T);
                return false;
            }

            result = __items.Dequeue();
            return true;
        }
    }

    // Reads the first item.
    public Boolean TryPeek(out T result)
    {
        lock (this)
        {
            if (__items.Count == 0)
            {
                result = default(T);
                return false;
            }

            result = __items.Peek();
            return true;
        }
    }

    public Void Clear()
    {
        lock (this)
            __items.Clear();
    }

    // A copy of the items, the first first.
    public T[] ToArray()
    {
        lock (this)
            return __items.ToArray();
    }

    // The enumerator goes through a copy.
    public IEnumerator<T> GetEnumerator()
    {
        return new ArrayEnumerator<T>(ToArray());
    }
}

////////// ////////// ////////// ////////// //////////

// A stack: last in, first out.
public class ConcurrentStack<T> : IReadOnlyCollection<T>
{
    private Stack<T> __items = new Stack<T>();

    public ConcurrentStack()
    {
    }

    public Int32 Count
    {
        get
        {
            lock (this)
                return __items.Count;
        }
    }

    public Boolean IsEmpty => Count == 0;

    public Void Push(T item)
    {
        lock (this)
            __items.Push(item);
    }

    // Pushes the items (the last one is the top), at once.
    public Void PushRange(Array<T> items)
    {
        lock (this)
        {
            for (Int32 k = 0; k < items.Length; k++)
                __items.Push(items[k]);
        }
    }

    // Takes the top: returns false when there is none.
    public Boolean TryPop(out T result)
    {
        lock (this)
        {
            if (__items.Count == 0)
            {
                result = default(T);
                return false;
            }

            result = __items.Pop();
            return true;
        }
    }

    public Boolean TryPeek(out T result)
    {
        lock (this)
        {
            if (__items.Count == 0)
            {
                result = default(T);
                return false;
            }

            result = __items.Peek();
            return true;
        }
    }

    public Void Clear()
    {
        lock (this)
            __items.Clear();
    }

    // A copy of the items, the top first.
    public T[] ToArray()
    {
        lock (this)
            return __items.ToArray();
    }

    public IEnumerator<T> GetEnumerator()
    {
        return new ArrayEnumerator<T>(ToArray());
    }
}

////////// ////////// ////////// ////////// //////////

// A bag of items that have no order (the last one that was added is taken first).
public class ConcurrentBag<T> : IReadOnlyCollection<T>
{
    private List<T> __items = new List<T>();

    public ConcurrentBag()
    {
    }

    public Int32 Count
    {
        get
        {
            lock (this)
                return __items.Count;
        }
    }

    public Boolean IsEmpty => Count == 0;

    public Void Add(T item)
    {
        lock (this)
            __items.Add(item);
    }

    public Boolean TryTake(out T result)
    {
        lock (this)
        {
            if (__items.Count == 0)
            {
                result = default(T);
                return false;
            }

            result = __items[__items.Count - 1];
            __items.RemoveAt(__items.Count - 1);
            return true;
        }
    }

    public Boolean TryPeek(out T result)
    {
        lock (this)
        {
            if (__items.Count == 0)
            {
                result = default(T);
                return false;
            }

            result = __items[__items.Count - 1];
            return true;
        }
    }

    public T[] ToArray()
    {
        lock (this)
            return __items.ToArray();
    }

    public IEnumerator<T> GetEnumerator()
    {
        return new ArrayEnumerator<T>(ToArray());
    }
}
