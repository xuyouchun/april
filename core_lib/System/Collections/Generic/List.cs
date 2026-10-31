/*
 *  System.dll
 *
 *  List<T>: a list of items.
 */

namespace Collections
{
    namespace Generic
    {
        ////////// ////////// ////////// ////////// //////////

        // A list of items, in the order they were added.
        //
        // A type has no default equality: Contains, IndexOf and Remove need the comparer that
        // is given to the constructor, they raise InvalidOperationException without it.
        public class List<T> : IList<T>, IReadOnlyList<T>
        {
            public List()
            {
                __items = new T[4];
                __count = 0;
            }

            // Creates a list with room for the specified number of items.
            public List(Int32 capacity)
            {
                if (capacity < 4)
                    capacity = 4;

                __items = new T[capacity];
                __count = 0;
            }

            // Creates a list that compares its items with the comparer.
            public List(IEqualityComparer<T> comparer)
            {
                __items = new T[4];
                __count = 0;
                __comparer = comparer;
            }

            // Items are not read only.
            public Boolean IsReadOnly
            {
                get { return false; }
            }

            // The number of items.
            public Int32 Count
            {
                get { return __count; }
            }

            // The item at the specified index.
            public T this[Int32 index]
            {
                get
                {
                    if (index < 0 || index >= __count)
                        throw new OutOfRangeException();

                    return __items[index];
                }

                set
                {
                    if (index < 0 || index >= __count)
                        throw new OutOfRangeException();

                    __items[index] = value;
                }
            }

            // Adds an item at the end.
            public Void Add(T item)
            {
                __reserve(__count + 1);

                __items[__count] = item;
                __count++;
            }

            // Inserts an item at the specified index.
            public Void Insert(Int32 index, T item)
            {
                if (index < 0 || index > __count)
                    throw new OutOfRangeException();

                __reserve(__count + 1);

                for (Int32 i = __count; i > index; i--)
                    __items[i] = __items[i - 1];

                __items[index] = item;
                __count++;
            }

            // Removes the item at the specified index.
            public Void RemoveAt(Int32 index)
            {
                if (index < 0 || index >= __count)
                    throw new OutOfRangeException();

                for (Int32 i = index; i < __count - 1; i++)
                    __items[i] = __items[i + 1];

                __count--;
                __items[__count] = __default;
            }

            // Removes all the items.
            public Void Clear()
            {
                for (Int32 i = 0; i < __count; i++)
                    __items[i] = __default;

                __count = 0;
            }

            // Returns an array of the items.
            public T[] ToArray()
            {
                T[] array = new T[__count];

                for (Int32 i = 0; i < __count; i++)
                    array[i] = __items[i];

                return array;
            }

            // Returns the index of the first item that equals it, -1 if there is no one.
            public Int32 IndexOf(T item)
            {
                IEqualityComparer<T> comparer = __need_comparer();

                for (Int32 i = 0; i < __count; i++)
                {
                    if (comparer.Equals(__items[i], item))
                        return i;
                }

                return -1;
            }

            // Returns whether the list has an item that equals it.
            public Boolean Contains(T item)
            {
                return IndexOf(item) >= 0;
            }

            // Removes the first item that equals it, returns false if there is no one.
            public Boolean Remove(T item)
            {
                Int32 index = IndexOf(item);
                if (index < 0)
                    return false;

                RemoveAt(index);
                return true;
            }

            // Copies the items to an array, starting at the index of the array.
            public Void CopyTo(T[] array, Int32 arrayIndex)
            {
                if (arrayIndex < 0 || arrayIndex + __count > array.Length)
                    throw new OutOfRangeException();

                for (Int32 i = 0; i < __count; i++)
                    array[arrayIndex + i] = __items[i];
            }

            // Returns the enumerator of the items, that foreach uses.
            public IEnumerator<T> GetEnumerator()
            {
                return new ListEnumerator<T>(this);
            }

            private IEqualityComparer<T> __need_comparer()
            {
                if (__comparer == null)
                    throw new InvalidOperationException("The list needs an equality comparer.");

                return __comparer;
            }

            private Void __reserve(Int32 count)
            {
                if (count <= __items.Length)
                    return;

                Int32 capacity = __items.Length * 2;
                if (capacity < count)
                    capacity = count;

                T[] items = new T[capacity];
                for (Int32 i = 0; i < __count; i++)
                    items[i] = __items[i];

                __items = items;
            }

            // The storage of the items: for the enumerator.
            public T[] __items;
            public Int32 __count;
            private T __default;        // The default value of T, the items that are removed.
            private IEqualityComparer<T> __comparer;
        }
    }
}
