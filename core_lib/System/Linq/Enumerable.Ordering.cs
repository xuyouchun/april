/*
 *  System.dll
 *
 *  System.Linq.Enumerable: the order of the items.
 *
 *  A type has no default order: the keys are compared by the comparer that is given. The sort
 *  is stable. The items are ordered when they are asked.
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

public static partial class Enumerable
{
    // The items, ordered by the keys that the selector gives.
    public static IOrderedEnumerable<T> OrderBy<T, TKey>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector, IComparer<TKey> comparer)
    {
        return new OrderedEnumerable<T>(source,
            new KeyComparer<T, TKey>(keySelector, comparer, false));
    }

    // The items, ordered by the keys, the greatest one first.
    public static IOrderedEnumerable<T> OrderByDescending<T, TKey>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector, IComparer<TKey> comparer)
    {
        return new OrderedEnumerable<T>(source,
            new KeyComparer<T, TKey>(keySelector, comparer, true));
    }

    // The items that are equal by the keys before, ordered by the keys that the selector gives.
    public static IOrderedEnumerable<T> ThenBy<T, TKey>(this IOrderedEnumerable<T> source,
        Delegate<TKey, T> keySelector, IComparer<TKey> comparer)
    {
        return source.CreateOrderedEnumerable<TKey>(keySelector, comparer, false);
    }

    // The items that are equal by the keys before, ordered by the keys, the greatest one first.
    public static IOrderedEnumerable<T> ThenByDescending<T, TKey>(this IOrderedEnumerable<T> source,
        Delegate<TKey, T> keySelector, IComparer<TKey> comparer)
    {
        return source.CreateOrderedEnumerable<TKey>(keySelector, comparer, true);
    }

    // The items, ordered by the keys that the selector gives: the keys are compared by the
    // default order of the type.
    public static IOrderedEnumerable<T> OrderBy<T, TKey>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector)
        => OrderBy<T, TKey>(source, keySelector, Comparer<TKey>.Default);

    // The items, ordered by the keys, the greatest one first.
    public static IOrderedEnumerable<T> OrderByDescending<T, TKey>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector)
        => OrderByDescending<T, TKey>(source, keySelector, Comparer<TKey>.Default);

    // The items that are equal by the keys before, ordered by the keys that the selector gives.
    public static IOrderedEnumerable<T> ThenBy<T, TKey>(this IOrderedEnumerable<T> source,
        Delegate<TKey, T> keySelector)
        => ThenBy<T, TKey>(source, keySelector, Comparer<TKey>.Default);

    // The items that are equal by the keys before, ordered by the keys, the greatest one first.
    public static IOrderedEnumerable<T> ThenByDescending<T, TKey>(this IOrderedEnumerable<T> source,
        Delegate<TKey, T> keySelector)
        => ThenByDescending<T, TKey>(source, keySelector, Comparer<TKey>.Default);
}
