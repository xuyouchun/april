/*
 *  System.dll
 *
 *  System.Linq.Enumerable: the sets of items, the concatenation and the reverse.
 *
 *  A type has no default equality: the sets are made with the comparer that is given.
 *  The items are in the order that they are found in.
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

public static partial class Enumerable
{
    // The items of the first collection, then those of the second one.
    public static IEnumerable<T> Concat<T>(this IEnumerable<T> first, IEnumerable<T> second)
    {
        foreach (T item in first)
            yield return item;

        foreach (T item in second)
            yield return item;
    }

    // The items, each one is told once.
    public static IEnumerable<T> Distinct<T>(this IEnumerable<T> source, IEqualityComparer<T> comparer)
    {
        Dictionary<T, Boolean> seen = new Dictionary<T, Boolean>(comparer);

        foreach (T item in source)
        {
            if (!seen.ContainsKey(item))
            {
                seen.Add(item, true);
                yield return item;
            }
        }
    }

    // The items of both collections, each one is told once.
    public static IEnumerable<T> Union<T>(this IEnumerable<T> first, IEnumerable<T> second,
                                          IEqualityComparer<T> comparer)
    {
        Dictionary<T, Boolean> seen = new Dictionary<T, Boolean>(comparer);

        foreach (T item in first)
        {
            if (!seen.ContainsKey(item))
            {
                seen.Add(item, true);
                yield return item;
            }
        }

        foreach (T item in second)
        {
            if (!seen.ContainsKey(item))
            {
                seen.Add(item, true);
                yield return item;
            }
        }
    }

    // The items of the first collection that the second one has, each one is told once.
    public static IEnumerable<T> Intersect<T>(this IEnumerable<T> first, IEnumerable<T> second,
                                              IEqualityComparer<T> comparer)
    {
        Dictionary<T, Boolean> others = new Dictionary<T, Boolean>(comparer);
        foreach (T item in second)
        {
            if (!others.ContainsKey(item))
                others.Add(item, true);
        }

        foreach (T item in first)
        {
            Boolean other;
            if (others.TryGetValue(item, out other) && other)
            {
                // It is told once.
                others.Set(item, false);
                yield return item;
            }
        }
    }

    // The items of the first collection that the second one has not, each one is told once.
    public static IEnumerable<T> Except<T>(this IEnumerable<T> first, IEnumerable<T> second,
                                           IEqualityComparer<T> comparer)
    {
        Dictionary<T, Boolean> seen = new Dictionary<T, Boolean>(comparer);
        foreach (T item in second)
        {
            if (!seen.ContainsKey(item))
                seen.Add(item, true);
        }

        foreach (T item in first)
        {
            if (!seen.ContainsKey(item))
            {
                seen.Add(item, true);
                yield return item;
            }
        }
    }

    // The items, from the last one to the first one.
    public static IEnumerable<T> Reverse<T>(this IEnumerable<T> source)
    {
        List<T> items = new List<T>();
        foreach (T item in source)
            items.Add(item);

        for (Int32 index = items.Count - 1; index >= 0; index--)
            yield return items[index];
    }

    // The items, each one is told once: the items are compared by the default equality of the type.
    public static IEnumerable<T> Distinct<T>(this IEnumerable<T> source)
        => Distinct<T>(source, EqualityComparer<T>.Default);

    // The items of both collections, each one is told once.
    public static IEnumerable<T> Union<T>(this IEnumerable<T> first, IEnumerable<T> second)
        => Union<T>(first, second, EqualityComparer<T>.Default);

    // The items of the first collection that the second one has, each one is told once.
    public static IEnumerable<T> Intersect<T>(this IEnumerable<T> first, IEnumerable<T> second)
        => Intersect<T>(first, second, EqualityComparer<T>.Default);

    // The items of the first collection that the second one has not, each one is told once.
    public static IEnumerable<T> Except<T>(this IEnumerable<T> first, IEnumerable<T> second)
        => Except<T>(first, second, EqualityComparer<T>.Default);
}
