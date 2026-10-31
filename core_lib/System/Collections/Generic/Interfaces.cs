/*
 *  System.dll
 *
 *  The interfaces of the collections, like those of .NET. There are no interfaces that
 *  are not generic: use IEnumerable<Object> instead of IEnumerable.
 *
 *  A type parameter that is only read is out (covariant), that is only written is in
 *  (contravariant): an IEnumerable<Dog> is an IEnumerable<Animal> and an IComparer<Animal>
 *  is an IComparer<Dog>. The variance is for the classes and the interfaces, not for the
 *  structs and strings.
 */

namespace Collections.Generic;

// Goes through the items of a collection.
public interface IEnumerator<out T> : IDisposable
{
    // Goes to the next item, returns false when there is no one.
    Boolean MoveNext();

    // The current item.
    T Current { get; }

    // Goes back to before the first item.
    Void Reset();
}

////////// ////////// ////////// ////////// //////////

// A collection that foreach goes through.
public interface IEnumerable<out T>
{
    IEnumerator<T> GetEnumerator();
}

////////// ////////// ////////// ////////// //////////

// A collection of items that are read.
public interface IReadOnlyCollection<out T> : IEnumerable<T>
{
    Int32 Count { get; }
}

////////// ////////// ////////// ////////// //////////

// A collection of items that are read by an index.
public interface IReadOnlyList<out T> : IReadOnlyCollection<T>
{
    T this[Int32 index] { get; }
}

////////// ////////// ////////// ////////// //////////

// A collection of items.
public interface ICollection<T> : IEnumerable<T>
{
    Int32 Count { get; }

    Boolean IsReadOnly { get; }

    Void Add(T item);

    Void Clear();

    // Whether the collection has the item. The collections need an equality comparer
    // to know it, a type has no default one.
    Boolean Contains(T item);

    // Copies the items to an array, starting at the index of the array.
    Void CopyTo(T[] array, Int32 arrayIndex);

    // Removes the first item that equals it, returns false if there is no one.
    Boolean Remove(T item);
}

////////// ////////// ////////// ////////// //////////

// A collection of items that are read and written by an index.
public interface IList<T> : ICollection<T>
{
    T this[Int32 index] { get; set; }

    // The index of the first item that equals it, -1 if there is no one.
    Int32 IndexOf(T item);

    Void Insert(Int32 index, T item);

    Void RemoveAt(Int32 index);
}

////////// ////////// ////////// ////////// //////////

// Orders the values of a type.
public interface IComparer<in T>
{
    // Returns a negative number if x is before y, zero if they are equal, a positive
    // number otherwise.
    Int32 Compare(T x, T y);
}

////////// ////////// ////////// ////////// //////////

// Compares the values of a type for equality.
public interface IEqualityComparer<in T>
{
    Boolean Equals(T x, T y);

    // The hash code of a value: the values that are equal have the same one.
    Int32 GetHashCode(T obj);
}

////////// ////////// ////////// ////////// //////////

// A key and its value: the items that a dictionary goes through.
public struct KeyValuePair<TKey, TValue>
{
    public KeyValuePair(TKey key, TValue value)
    {
        __key = key;
        __value = value;
    }

    public TKey Key
    {
        get { return __key; }
    }

    public TValue Value
    {
        get { return __value; }
    }

    private TKey __key;
    private TValue __value;
}

////////// ////////// ////////// ////////// //////////

// A dictionary that is read: a value is found by its key.
public interface IReadOnlyDictionary<TKey, TValue> : IEnumerable<KeyValuePair<TKey, TValue>>
{
    Int32 Count { get; }

    // The value of the key, KeyNotFoundException if there is no one.
    TValue this[TKey key] { get; }

    Boolean ContainsKey(TKey key);

    // Gets the value of the key, returns false if there is no one.
    Boolean TryGetValue(TKey key, out TValue value);

    // The keys, a copy of them.
    IReadOnlyCollection<TKey> Keys { get; }

    // The values, a copy of them.
    IReadOnlyCollection<TValue> Values { get; }
}

////////// ////////// ////////// ////////// //////////

// A dictionary: the values are found, added and removed by their keys.
public interface IDictionary<TKey, TValue> : IReadOnlyDictionary<TKey, TValue>
{
    Boolean IsReadOnly { get; }

    // Adds a key, InvalidOperationException if it is in the dictionary already.
    Void Add(TKey key, TValue value);

    // Sets the value of the key, adds it if there is no one.
    Void Set(TKey key, TValue value);

    // Removes the key, returns false if there is no one.
    Boolean Remove(TKey key);

    Void Clear();
}
