/*
 *  System.dll
 *
 *  Dictionary<TKey, TValue>: a hash table, and the exception of the missing keys.
 */

namespace Collections.Generic;

////////// ////////// ////////// ////////// //////////

// A key is not in the dictionary.
public class KeyNotFoundException : SystemException
{
    // Default constructor.
    public KeyNotFoundException() { }

    // Constructor with message.
    public KeyNotFoundException(String message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// A hash table of keys and values.
//
// A type has no default equality and hash code: the dictionary needs the comparer that is
// given to the constructor.
public class Dictionary<TKey, TValue> : IDictionary<TKey, TValue>
{
    // Creates a dictionary that compares its keys with the comparer.
    public Dictionary(IEqualityComparer<TKey> comparer)
    {
        if (comparer == null)
            throw new InvalidOperationException("The dictionary needs an equality comparer.");

        __comparer = comparer;
        __initialize(4);
    }

    // Creates a dictionary with room for the specified number of items.
    public Dictionary(IEqualityComparer<TKey> comparer, Int32 capacity)
    {
        if (comparer == null)
            throw new InvalidOperationException("The dictionary needs an equality comparer.");

        Int32 size = 4;
        while (size < capacity)
            size = size * 2;

        __comparer = comparer;
        __initialize(size);
    }

    // The number of keys.
    public Int32 Count
    {
        get { return __count - __freeCount; }
    }

    public Boolean IsReadOnly
    {
        get { return false; }
    }

    // The value of the key, KeyNotFoundException if there is no one. Setting it adds the
    // key if there is no one.
    public TValue this[TKey key]
    {
        get
        {
            Int32 index = __find(key);
            if (index < 0)
                throw new KeyNotFoundException("The key is not in the dictionary.");

            return __values[index];
        }

        set
        {
            __insert(key, value, false);
        }
    }

    // Adds a key, InvalidOperationException if it is in the dictionary already.
    public Void Add(TKey key, TValue value)
    {
        __insert(key, value, true);
    }

    // Sets the value of the key, adds it if there is no one.
    public Void Set(TKey key, TValue value)
    {
        __insert(key, value, false);
    }

    public Boolean ContainsKey(TKey key)
    {
        return __find(key) >= 0;
    }

    // Gets the value of the key, returns false if there is no one.
    public Boolean TryGetValue(TKey key, out TValue value)
    {
        Int32 index = __find(key);
        if (index < 0)
        {
            value = __defaultValue;
            return false;
        }

        value = __values[index];
        return true;
    }

    // Removes the key, returns false if there is no one.
    public Boolean Remove(TKey key)
    {
        Int32 hash = __hash_of(key);
        Int32 bucket = hash & (__buckets.Length - 1);
        Int32 previous = -1;

        for (Int32 i = __buckets[bucket] - 1; i >= 0; i = __next[i])
        {
            if (__hashes[i] == hash && __comparer.Equals(__keys[i], key))
            {
                if (previous < 0)
                    __buckets[bucket] = __next[i] + 1;
                else
                    __next[previous] = __next[i];

                __hashes[i] = -1;
                __keys[i] = __defaultKey;
                __values[i] = __defaultValue;

                // The slot goes to the list of the free ones.
                __next[i] = __freeList;
                __freeList = i;
                __freeCount++;

                return true;
            }

            previous = i;
        }

        return false;
    }

    // Removes all the keys.
    public Void Clear()
    {
        for (Int32 i = 0; i < __buckets.Length; i++)
            __buckets[i] = 0;

        for (Int32 i = 0; i < __count; i++)
        {
            __hashes[i] = -1;
            __keys[i] = __defaultKey;
            __values[i] = __defaultValue;
        }

        __count = 0;
        __freeList = -1;
        __freeCount = 0;
    }

    // The keys, a copy of them.
    public IReadOnlyCollection<TKey> Keys
    {
        get
        {
            List<TKey> keys = new List<TKey>(__count);
            for (Int32 i = 0; i < __count; i++)
            {
                if (__hashes[i] >= 0)
                    keys.Add(__keys[i]);
            }

            return keys;
        }
    }

    // The values, a copy of them.
    public IReadOnlyCollection<TValue> Values
    {
        get
        {
            List<TValue> values = new List<TValue>(__count);
            for (Int32 i = 0; i < __count; i++)
            {
                if (__hashes[i] >= 0)
                    values.Add(__values[i]);
            }

            return values;
        }
    }

    // Returns the enumerator of the items, that foreach uses.
    public IEnumerator<KeyValuePair<TKey, TValue>> GetEnumerator()
    {
        return new DictionaryEnumerator<TKey, TValue>(this);
    }

    // Returns the index of the key, -1 if there is no one.
    private Int32 __find(TKey key)
    {
        Int32 hash = __hash_of(key);

        for (Int32 i = __buckets[hash & (__buckets.Length - 1)] - 1; i >= 0; i = __next[i])
        {
            if (__hashes[i] == hash && __comparer.Equals(__keys[i], key))
                return i;
        }

        return -1;
    }

    // The hash code of a key, it is not negative: a free slot has -1.
    private Int32 __hash_of(TKey key)
    {
        return __comparer.GetHashCode(key) & 2147483647;
    }

    private Void __insert(TKey key, TValue value, Boolean add)
    {
        Int32 hash = __hash_of(key);
        Int32 bucket = hash & (__buckets.Length - 1);

        for (Int32 i = __buckets[bucket] - 1; i >= 0; i = __next[i])
        {
            if (__hashes[i] == hash && __comparer.Equals(__keys[i], key))
            {
                if (add)
                    throw new InvalidOperationException("The key is in the dictionary already.");

                __values[i] = value;
                return;
            }
        }

        Int32 index;
        if (__freeCount > 0)
        {
            index = __freeList;
            __freeList = __next[index];
            __freeCount--;
        }
        else
        {
            if (__count == __keys.Length)
            {
                __resize();
                bucket = hash & (__buckets.Length - 1);
            }

            index = __count;
            __count++;
        }

        __hashes[index] = hash;
        __keys[index] = key;
        __values[index] = value;
        __next[index] = __buckets[bucket] - 1;
        __buckets[bucket] = index + 1;
    }

    // Doubles the size of the tables, and puts the keys in the buckets again.
    private Void __resize()
    {
        Int32 size = __keys.Length * 2;

        Int32[] hashes = new Int32[size];
        Int32[] next = new Int32[size];
        TKey[] keys = new TKey[size];
        TValue[] values = new TValue[size];

        for (Int32 i = 0; i < __count; i++)
        {
            hashes[i] = __hashes[i];
            keys[i] = __keys[i];
            values[i] = __values[i];
        }

        Int32[] buckets = new Int32[size];
        for (Int32 i = 0; i < __count; i++)
        {
            if (hashes[i] >= 0)
            {
                Int32 bucket = hashes[i] & (size - 1);
                next[i] = buckets[bucket] - 1;
                buckets[bucket] = i + 1;
            }
            else
            {
                next[i] = __next[i];
            }
        }

        __hashes = hashes;
        __next = next;
        __keys = keys;
        __values = values;
        __buckets = buckets;
    }

    private Void __initialize(Int32 size)
    {
        __buckets = new Int32[size];
        __next = new Int32[size];
        __hashes = new Int32[size];
        __keys = new TKey[size];
        __values = new TValue[size];

        __count = 0;
        __freeList = -1;
        __freeCount = 0;
    }

    private Int32[] __buckets;          // The first index + 1 of each hash, 0 for none.
    private Int32[] __next;             // The next index of the chain, or of the free slots.

    // The slots, for the enumerator: the generic code of a type does not call a method that
    // returns T, it reads the arrays.
    public Int32[] __hashes;            // The hash of each item, -1 for a free slot.
    public TKey[] __keys;
    public TValue[] __values;
    public Int32 __count;               // The slots that are used or were used.

    private Int32 __freeList;           // The first free slot, -1 for none.
    private Int32 __freeCount;
    private IEqualityComparer<TKey> __comparer;
    private TKey __defaultKey;          // The default values, for the slots that are freed.
    private TValue __defaultValue;
}

////////// ////////// ////////// ////////// //////////

// The enumerator of a dictionary.
public class DictionaryEnumerator<TKey, TValue> : IEnumerator<KeyValuePair<TKey, TValue>>
{
    public DictionaryEnumerator(Dictionary<TKey, TValue> dictionary)
    {
        __dictionary = dictionary;
        __index = -1;
    }

    // Goes to the next item, returns false at the end.
    public Boolean MoveNext()
    {
        while (__index + 1 < __dictionary.__count)
        {
            __index++;
            if (__dictionary.__hashes[__index] >= 0)
                return true;
        }

        return false;
    }

    // The current item.
    public KeyValuePair<TKey, TValue> Current
    {
        get
        {
            return new KeyValuePair<TKey, TValue>(
                __dictionary.__keys[__index], __dictionary.__values[__index]
            );
        }
    }

    // Goes back to before the first item.
    public Void Reset()
    {
        __index = -1;
    }

    // Nothing to release.
    public Void Dispose()
    {
    }

    private Dictionary<TKey, TValue> __dictionary;
    private Int32 __index;
}
