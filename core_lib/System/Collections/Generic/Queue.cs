/*
 *  System.dll
 *
 *  Queue<T>: a first-in first-out collection.
 */

namespace Collections
{
    namespace Generic
    {
        ////////// ////////// ////////// ////////// //////////

        // A first-in first-out collection.
        public class Queue<T> : IReadOnlyCollection<T>
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

            // Returns the items, from the front.
            public T[] ToArray()
            {
                T[] array = new T[__count];

                for (Int32 i = 0; i < __count; i++)
                    array[i] = __items[(__head + i) % __items.Length];

                return array;
            }

            // Returns the enumerator of a copy of the items, from the front.
            public IEnumerator<T> GetEnumerator()
            {
                return new ArrayEnumerator<T>(ToArray());
            }

            private T[] __items;
            private Int32 __head;
            private Int32 __count;
            private T __default;
        }
    }
}
