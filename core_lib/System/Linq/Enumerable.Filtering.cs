/*
 *  System.dll
 *
 *  System.Linq.Enumerable: the filtering of the items of a collection.
 *
 *  The methods are extension methods of IEnumerable<T>. The ones that make a collection
 *  are iterators: nothing is done until the items are asked, and the source is gone through
 *  once (deferred execution).
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

public static partial class Enumerable
{
    // The items that the predicate is true for.
    public static IEnumerable<T> Where<T>(this IEnumerable<T> source, Delegate<Boolean, T> predicate)
    {
        foreach (T item in source)
        {
            if (predicate(item))
                yield return item;
        }
    }

    // The items that the predicate is true for, with the index of the item in the source.
    public static IEnumerable<T> Where<T>(this IEnumerable<T> source,
                                          Delegate<Boolean, T, Int32> predicate)
    {
        Int32 index = 0;
        foreach (T item in source)
        {
            if (predicate(item, index))
                yield return item;

            index++;
        }
    }

    // The first count items.
    public static IEnumerable<T> Take<T>(this IEnumerable<T> source, Int32 count)
    {
        if (count <= 0)
            yield break;

        // It stops when it has the items: the next one of the source is not asked.
        Int32 taken = 0;
        foreach (T item in source)
        {
            taken++;
            yield return item;

            if (taken >= count)
                yield break;
        }
    }

    // The items before the first one that the predicate is false for.
    public static IEnumerable<T> TakeWhile<T>(this IEnumerable<T> source,
                                              Delegate<Boolean, T> predicate)
    {
        foreach (T item in source)
        {
            if (!predicate(item))
                yield break;

            yield return item;
        }
    }

    // The items after the first count items.
    public static IEnumerable<T> Skip<T>(this IEnumerable<T> source, Int32 count)
    {
        Int32 index = 0;
        foreach (T item in source)
        {
            if (index >= count)
                yield return item;

            index++;
        }
    }

    // The items from the first one that the predicate is false for.
    public static IEnumerable<T> SkipWhile<T>(this IEnumerable<T> source,
                                              Delegate<Boolean, T> predicate)
    {
        Boolean skipping = true;
        foreach (T item in source)
        {
            if (skipping && !predicate(item))
                skipping = false;

            if (!skipping)
                yield return item;
        }
    }
}
