/*
 *  System.dll
 *
 *  The interfaces of the collections, like those of .NET. There are no interfaces that are not
 *  generic: use IEnumerable<Object> instead of IEnumerable.
 *
 *  A type parameter that is only read is out (covariant), that is only written is in
 *  (contravariant): an IEnumerable<Dog> is an IEnumerable<Animal> and an IComparer<Animal>
 *  is an IComparer<Dog>. The variance is for the classes and the interfaces, not for the
 *  structs and strings.
 */

////////// ////////// ////////// ////////// //////////

// A type that releases its resources: the using statement calls Dispose() on exit, and
// foreach calls it on its enumerator.
public interface IDisposable
{
    Void Dispose();
}

////////// ////////// ////////// ////////// //////////

// A type whose values are ordered.
public interface IComparable<in T>
{
    // Returns a negative number if this is before other, zero if they are equal, a positive
    // number otherwise.
    Int32 CompareTo(T other);
}

////////// ////////// ////////// ////////// //////////

// A type whose values are compared for equality.
public interface IEquatable<in T>
{
    Boolean Equals(T other);
}

////////// ////////// ////////// ////////// //////////

namespace Collections
{
    namespace Generic
    {
        ////////// ////////// ////////// ////////// //////////

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
    }
}
