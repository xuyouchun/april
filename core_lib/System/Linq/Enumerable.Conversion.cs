/*
 *  System.dll
 *
 *  System.Linq.Enumerable: the conversion of a collection to another one.
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

public static partial class Enumerable
{
    // A list of the items.
    public static List<T> ToList<T>(this IEnumerable<T> source)
    {
        List<T> list = new List<T>();
        foreach (T item in source)
            list.Add(item);

        return list;
    }

    // An array of the items.
    public static T[] ToArray<T>(this IEnumerable<T> source)
    {
        List<T> list = new List<T>();
        foreach (T item in source)
            list.Add(item);

        return list.ToArray();
    }

    // A dictionary of the items, by the keys that the selector gives: InvalidOperationException
    // if a key is found twice.
    public static Dictionary<TKey, T> ToDictionary<T, TKey>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector, IEqualityComparer<TKey> comparer)
    {
        Dictionary<TKey, T> dictionary = new Dictionary<TKey, T>(comparer);
        foreach (T item in source)
        {
            TKey key = keySelector(item);
            dictionary.Add(key, item);
        }

        return dictionary;
    }

    // A dictionary of the results of the element selector, by the keys of the items.
    public static Dictionary<TKey, TElement> ToDictionary<T, TKey, TElement>(
        this IEnumerable<T> source, Delegate<TKey, T> keySelector,
        Delegate<TElement, T> elementSelector, IEqualityComparer<TKey> comparer)
    {
        Dictionary<TKey, TElement> dictionary = new Dictionary<TKey, TElement>(comparer);
        foreach (T item in source)
        {
            TKey key = keySelector(item);
            TElement element = elementSelector(item);
            dictionary.Add(key, element);
        }

        return dictionary;
    }

    // A dictionary of the items, by the keys that the selector gives: the keys are compared by
    // the default equality of the type.
    public static Dictionary<TKey, T> ToDictionary<T, TKey>(this IEnumerable<T> source,
        Delegate<TKey, T> keySelector)
        => ToDictionary<T, TKey>(source, keySelector, EqualityComparer<TKey>.Default);

    // A dictionary of the results of the element selector, by the keys of the items.
    public static Dictionary<TKey, TElement> ToDictionary<T, TKey, TElement>(
        this IEnumerable<T> source, Delegate<TKey, T> keySelector,
        Delegate<TElement, T> elementSelector)
        => ToDictionary<T, TKey, TElement>(source, keySelector, elementSelector,
                                           EqualityComparer<TKey>.Default);
}
