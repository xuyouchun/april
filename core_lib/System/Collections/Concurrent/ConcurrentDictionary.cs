/*
 *  System.dll
 *
 *  ConcurrentDictionary<TKey, TValue>: a dictionary that many threads use; each call holds the lock of the dictionary.
 */

using Threading;

namespace Collections.Concurrent;

////////// ////////// ////////// ////////// //////////

// The factories that GetOrAdd and AddOrUpdate take run out of the lock, so they can use the dictionary; when another thread
// added the key meanwhile, its value is the one that is kept.
public class ConcurrentDictionary<TKey, TValue> : IEnumerable<KeyValuePair<TKey, TValue>>
{
    private Dictionary<TKey, TValue> __items = new Dictionary<TKey, TValue>();

    public ConcurrentDictionary()
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

    // The value of the key, KeyNotFoundException if there is none; the setter adds or replaces.
    public TValue this[TKey key]
    {
        get
        {
            lock (this)
                return __items[key];
        }

        set
        {
            lock (this)
                __items.Set(key, value);
        }
    }

    public Boolean ContainsKey(TKey key)
    {
        lock (this)
            return __items.ContainsKey(key);
    }

    // Adds the pair: returns false when the key is there.
    public Boolean TryAdd(TKey key, TValue value)
    {
        lock (this)
        {
            if (__items.ContainsKey(key))
                return false;

            __items.Add(key, value);
            return true;
        }
    }

    public Boolean TryGetValue(TKey key, out TValue value)
    {
        lock (this)
            return __items.TryGetValue(key, out value);
    }

    // Removes the key: returns false when it was not there, the value that it had is the result.
    public Boolean TryRemove(TKey key, out TValue value)
    {
        lock (this)
        {
            if (!__items.TryGetValue(key, out value))
                return false;

            __items.Remove(key);
            return true;
        }
    }

    // Replaces the value if it is the compared one: returns whether it did.
    public Boolean TryUpdate(TKey key, TValue newValue, TValue comparisonValue)
    {
        lock (this)
        {
            TValue current;
            if (!__items.TryGetValue(key, out current))
                return false;

            if (!EqualityComparer<TValue>.Default.Equals(current, comparisonValue))
                return false;

            __items.Set(key, newValue);
            return true;
        }
    }

    // The value of the key, or the new value that was added.
    public TValue GetOrAdd(TKey key, TValue value)
    {
        lock (this)
        {
            TValue current;
            if (__items.TryGetValue(key, out current))
                return current;

            __items.Add(key, value);
            return value;
        }
    }

    public TValue GetOrAdd(TKey key, Delegate<TValue, TKey> valueFactory)
    {
        TValue existing;
        if (TryGetValue(key, out existing))
            return existing;

        TValue made = valueFactory(key);
        return GetOrAdd(key, made);
    }

    // Adds the pair, or replaces the value by the one that the method makes from the key and the value that it has.
    public TValue AddOrUpdate(TKey key, TValue addValue, Delegate<TValue, TKey, TValue> updateValueFactory)
    {
        for (;;)
        {
            TValue current;
            if (TryGetValue(key, out current))
            {
                TValue updated = updateValueFactory(key, current);
                if (TryUpdate(key, updated, current))
                    return updated;
            }
            else if (TryAdd(key, addValue))
            {
                return addValue;
            }
        }
    }

    public TValue AddOrUpdate(TKey key, Delegate<TValue, TKey> addValueFactory, Delegate<TValue, TKey, TValue> updateValueFactory)
    {
        for (;;)
        {
            TValue current;
            if (TryGetValue(key, out current))
            {
                TValue updated = updateValueFactory(key, current);
                if (TryUpdate(key, updated, current))
                    return updated;
            }
            else
            {
                TValue added = addValueFactory(key);
                if (TryAdd(key, added))
                    return added;
            }
        }
    }

    public Void Clear()
    {
        lock (this)
            __items.Clear();
    }

    // A copy of the keys.
    public TKey[] Keys
    {
        get
        {
            lock (this)
                return __Keys();
        }
    }

    // A copy of the values.
    public TValue[] Values
    {
        get
        {
            lock (this)
                return __Values();
        }
    }

    // A copy of the pairs.
    public KeyValuePair<TKey, TValue>[] ToArray()
    {
        lock (this)
        {
            KeyValuePair<TKey, TValue>[] result = new KeyValuePair<TKey, TValue>[__items.Count];
            Int32 k = 0;

            IEnumerator<KeyValuePair<TKey, TValue>> enumerator = __items.GetEnumerator();
            while (enumerator.MoveNext())
                result[k++] = enumerator.Current;

            return result;
        }
    }

    // The enumerator goes through a copy.
    public IEnumerator<KeyValuePair<TKey, TValue>> GetEnumerator()
    {
        return new ArrayEnumerator<KeyValuePair<TKey, TValue>>(ToArray());
    }

    private TKey[] __Keys()
    {
        TKey[] result = new TKey[__items.Count];
        Int32 k = 0;

        IEnumerator<KeyValuePair<TKey, TValue>> enumerator = __items.GetEnumerator();
        while (enumerator.MoveNext())
            result[k++] = enumerator.Current.Key;

        return result;
    }

    private TValue[] __Values()
    {
        TValue[] result = new TValue[__items.Count];
        Int32 k = 0;

        IEnumerator<KeyValuePair<TKey, TValue>> enumerator = __items.GetEnumerator();
        while (enumerator.MoveNext())
            result[k++] = enumerator.Current.Value;

        return result;
    }
}
