/*
 *  System.dll
 *
 *  Stack<T>: a last-in first-out collection.
 */

namespace Collections
{
    namespace Generic
    {
        ////////// ////////// ////////// ////////// //////////

        // A last-in first-out collection.
        public class Stack<T> : IReadOnlyCollection<T>
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

            // Returns the items, from the top.
            public T[] ToArray()
            {
                T[] array = new T[__count];

                for (Int32 i = 0; i < __count; i++)
                    array[i] = __items[__count - 1 - i];

                return array;
            }

            // Returns the enumerator of a copy of the items, from the top.
            public IEnumerator<T> GetEnumerator()
            {
                return new ArrayEnumerator<T>(ToArray());
            }

            private T[] __items;
            private Int32 __count;
            private T __default;
        }
    }
}
