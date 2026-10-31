/*
 *  System.dll
 *
 *  System.Collections.Generic: lists, stacks and queues.
 *
 *  The generic code that reads the items of a T[] does not call a method that returns T:
 *  the result of such a call is not known to be a value or a struct before T is.
 */

namespace Collections
{
    namespace Generic
    {
        ////////// ////////// ////////// ////////// //////////

        // A list of items, in the order they were added.
        public class List<T>
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

            // Returns the enumerator of the items, that foreach uses.
            public ListEnumerator<T> GetEnumerator()
            {
                return new ListEnumerator<T>(this);
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
        }

        ////////// ////////// ////////// ////////// //////////

        // The enumerator of a list.
        public class ListEnumerator<T>
        {
            public ListEnumerator(List<T> list)
            {
                __list = list;
                __index = -1;
            }

            // Goes to the next item, returns false at the end.
            public Boolean MoveNext()
            {
                if (__index + 1 >= __list.__count)
                    return false;

                __index++;
                return true;
            }

            // The current item.
            public T Current
            {
                get { return __list.__items[__index]; }
            }

            private List<T> __list;
            private Int32 __index;
        }

        ////////// ////////// ////////// ////////// //////////

        // A last-in first-out collection.
        public class Stack<T>
        {
            public Stack()
            {
                __items = new T[4];
                __count = 0;
            }

            public Int32 Count
            {
                get { return __count; }
            }

            // Adds an item on the top.
            public Void Push(T item)
            {
                if (__count == __items.Length)
                {
                    T[] items = new T[__count * 2];
                    for (Int32 i = 0; i < __count; i++)
                        items[i] = __items[i];

                    __items = items;
                }

                __items[__count] = item;
                __count++;
            }

            // Removes the item on the top, and returns it.
            public T Pop()
            {
                if (__count == 0)
                    throw new InvalidOperationException("The stack is empty.");

                __count--;

                T item = __items[__count];
                __items[__count] = __default;

                return item;
            }

            // Returns the item on the top.
            public T Peek()
            {
                if (__count == 0)
                    throw new InvalidOperationException("The stack is empty.");

                return __items[__count - 1];
            }

            public Void Clear()
            {
                for (Int32 i = 0; i < __count; i++)
                    __items[i] = __default;

                __count = 0;
            }

            private T[] __items;
            private Int32 __count;
            private T __default;
        }

        ////////// ////////// ////////// ////////// //////////

        // A first-in first-out collection.
        public class Queue<T>
        {
            public Queue()
            {
                __items = new T[4];
                __head = 0;
                __count = 0;
            }

            public Int32 Count
            {
                get { return __count; }
            }

            // Adds an item at the end.
            public Void Enqueue(T item)
            {
                if (__count == __items.Length)
                {
                    // The items are copied from the head, the queue starts at 0 again.
                    T[] items = new T[__count * 2];
                    for (Int32 i = 0; i < __count; i++)
                        items[i] = __items[(__head + i) % __items.Length];

                    __items = items;
                    __head = 0;
                }

                __items[(__head + __count) % __items.Length] = item;
                __count++;
            }

            // Removes the item at the front, and returns it.
            public T Dequeue()
            {
                if (__count == 0)
                    throw new InvalidOperationException("The queue is empty.");

                T item = __items[__head];
                __items[__head] = __default;

                __head = (__head + 1) % __items.Length;
                __count--;

                return item;
            }

            // Returns the item at the front.
            public T Peek()
            {
                if (__count == 0)
                    throw new InvalidOperationException("The queue is empty.");

                return __items[__head];
            }

            public Void Clear()
            {
                for (Int32 i = 0; i < __items.Length; i++)
                    __items[i] = __default;

                __head = 0;
                __count = 0;
            }

            private T[] __items;
            private Int32 __head;
            private Int32 __count;
            private T __default;
        }
    }
}
