/*
 *  System.dll
 *
 *  System.Linq.Enumerable: the groups of the items that have the same key.
 *
 *  The groups are in the order that their keys are found in, the items of a group are in the
 *  order of the source.
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

public static partial class Enumerable
{
    // The lookup of the items, by their keys.
    public static ILookup<TKey, T> ToLookup<T, TKey>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector, IEqualityComparer<TKey> comparer)
    {
        Lookup<TKey, T> lookup = new Lookup<TKey, T>(comparer);
        foreach (T item in source)
        {
            TKey key = keySelector(item);
            lookup.Add(key, item);
        }

        return lookup;
    }

    // The lookup of the results of the element selector, by the keys of the items.
    public static ILookup<TKey, TElement> ToLookup<T, TKey, TElement>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector, Delegate<TElement, T> elementSelector,
        IEqualityComparer<TKey> comparer)
    {
        Lookup<TKey, TElement> lookup = new Lookup<TKey, TElement>(comparer);
        foreach (T item in source)
        {
            TKey key = keySelector(item);
            TElement element = elementSelector(item);
            lookup.Add(key, element);
        }

        return lookup;
    }

    // The groups of the items that have the same key.
    public static IEnumerable<IGrouping<TKey, T>> GroupBy<T, TKey>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector, IEqualityComparer<TKey> comparer)
    {
        ILookup<TKey, T> lookup = ToLookup<T, TKey>(source, keySelector, comparer);
        foreach (IGrouping<TKey, T> group in lookup)
            yield return group;
    }

    // The groups of the results of the element selector, by the keys of the items.
    public static IEnumerable<IGrouping<TKey, TElement>> GroupBy<T, TKey, TElement>(
        this IEnumerable<T> source, Delegate<TKey, T> keySelector,
        Delegate<TElement, T> elementSelector, IEqualityComparer<TKey> comparer)
    {
        ILookup<TKey, TElement> lookup = ToLookup<T, TKey, TElement>(source, keySelector,
                                                                     elementSelector, comparer);
        foreach (IGrouping<TKey, TElement> group in lookup)
            yield return group;
    }

    // The result of the result selector for each group.
    public static IEnumerable<TResult> GroupBy<T, TKey, TResult>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector, Delegate<TResult, TKey, IEnumerable<T>> resultSelector,
        IEqualityComparer<TKey> comparer)
    {
        ILookup<TKey, T> lookup = ToLookup<T, TKey>(source, keySelector, comparer);
        foreach (IGrouping<TKey, T> group in lookup)
        {
            TKey key = group.Key;
            TResult result = resultSelector(key, group);
            yield return result;
        }
    }

    // The lookup of the items, by their keys: the keys are compared by the default equality.
    public static ILookup<TKey, T> ToLookup<T, TKey>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector)
        => ToLookup<T, TKey>(source, keySelector, EqualityComparer<TKey>.Default);

    // The lookup of the results of the element selector, by the keys of the items.
    public static ILookup<TKey, TElement> ToLookup<T, TKey, TElement>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector, Delegate<TElement, T> elementSelector)
        => ToLookup<T, TKey, TElement>(source, keySelector, elementSelector,
                                       EqualityComparer<TKey>.Default);

    // The groups of the items that have the same key.
    public static IEnumerable<IGrouping<TKey, T>> GroupBy<T, TKey>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector)
        => GroupBy<T, TKey>(source, keySelector, EqualityComparer<TKey>.Default);

    // The groups of the results of the element selector, by the keys of the items.
    public static IEnumerable<IGrouping<TKey, TElement>> GroupBy<T, TKey, TElement>(
        this IEnumerable<T> source, Delegate<TKey, T> keySelector,
        Delegate<TElement, T> elementSelector)
        => GroupBy<T, TKey, TElement>(source, keySelector, elementSelector,
                                      EqualityComparer<TKey>.Default);

    // The result of the result selector for each group.
    public static IEnumerable<TResult> GroupBy<T, TKey, TResult>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector, Delegate<TResult, TKey, IEnumerable<T>> resultSelector)
        => GroupBy<T, TKey, TResult>(source, keySelector, resultSelector,
                                     EqualityComparer<TKey>.Default);
}
