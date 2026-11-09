/*
 *  System.dll
 *
 *  System.Linq: the ordering of the items. The items are ordered when they are asked, by a
 *  sort that is stable: the items that are equal are in the order of the source.
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

// Compares the items by a key.
public class KeyComparer<T, TKey> : IComparer<T>
{
    public KeyComparer(Delegate<TKey, T> keySelector, IComparer<TKey> comparer, Boolean descending)
    {
        __keySelector = keySelector;
        __comparer = comparer;
        __descending = descending;
    }

    public Int32 Compare(T x, T y)
    {
        TKey keyX = __keySelector(x);
        TKey keyY = __keySelector(y);

        Int32 result = __comparer.Compare(keyX, keyY);
        return __descending? -result : result;
    }

    private Delegate<TKey, T> __keySelector;
    private IComparer<TKey> __comparer;
    private Boolean __descending;
}

// Compares the items by a comparer, and by another one when the items are equal.
public class ThenComparer<T> : IComparer<T>
{
    public ThenComparer(IComparer<T> first, IComparer<T> second)
    {
        __first = first;
        __second = second;
    }

    public Int32 Compare(T x, T y)
    {
        Int32 result = __first.Compare(x, y);
        if (result != 0)
            return result;

        return __second.Compare(x, y);
    }

    private IComparer<T> __first;
    private IComparer<T> __second;
}

////////// ////////// ////////// ////////// //////////

// The items of a collection, ordered by a comparer.
public class OrderedEnumerable<T> : IOrderedEnumerable<T>
{
    public OrderedEnumerable(IEnumerable<T> source, IComparer<T> comparer)
    {
        __source = source;
        __comparer = comparer;
    }

    // The collection that is ordered by the key, for the items that are equal by this one.
    public IOrderedEnumerable<T> CreateOrderedEnumerable<TKey>(Delegate<TKey, T> keySelector,
                                                               IComparer<TKey> comparer,
                                                               Boolean descending)
    {
        KeyComparer<T, TKey> keyComparer = new KeyComparer<T, TKey>(keySelector, comparer,
                                                                    descending);

        return new OrderedEnumerable<T>(__source, new ThenComparer<T>(__comparer, keyComparer));
    }

    public IEnumerator<T> GetEnumerator()
    {
        List<T> items = new List<T>();
        foreach (T item in __source)
            items.Add(item);

        T[] sorted = Sorting.Sort<T>(items.ToArray(), __comparer);
        return new ArrayEnumerator<T>(sorted);
    }

    private IEnumerable<T> __source;
    private IComparer<T> __comparer;
}

////////// ////////// ////////// ////////// //////////

// The stable sort of an array.
public static class Sorting
{
    // The items of the array, in the order of the comparer. (A merge sort.)
    public static T[] Sort<T>(T[] items, IComparer<T> comparer)
    {
        Int32 count = items.Length;
        if (count < 2)
            return items;

        T[] source = items;
        T[] target = new T[count];

        // The runs of the width are merged to the runs of the double width.
        for (Int32 width = 1; width < count; width = width * 2)
        {
            for (Int32 start = 0; start < count; start = start + 2 * width)
            {
                Int32 middle = Math.Min(start + width, count);
                Int32 end = Math.Min(start + 2 * width, count);

                __Merge<T>(source, target, start, middle, end, comparer);
            }

            T[] swap = source;
            source = target;
            target = swap;
        }

        return source;
    }

    // Merges the runs [start, middle) and [middle, end) of the source to the target.
    private static Void __Merge<T>(T[] source, T[] target, Int32 start, Int32 middle,
                                   Int32 end, IComparer<T> comparer)
    {
        Int32 left = start;
        Int32 right = middle;

        for (Int32 index = start; index < end; index++)
        {
            Boolean takeLeft;
            if (left >= middle)
            {
                takeLeft = false;
            }
            else if (right >= end)
            {
                takeLeft = true;
            }
            else
            {
                T leftItem = source[left];
                T rightItem = source[right];

                // The left one first when they are equal: the sort is stable.
                takeLeft = comparer.Compare(leftItem, rightItem) <= 0;
            }

            if (takeLeft)
            {
                target[index] = source[left];
                left++;
            }
            else
            {
                target[index] = source[right];
                right++;
            }
        }
    }
}
