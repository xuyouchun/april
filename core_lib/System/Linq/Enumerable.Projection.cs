/*
 *  System.dll
 *
 *  System.Linq.Enumerable: the projection of the items of a collection.
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

public static partial class Enumerable
{
    // The result of the selector for each item.
    public static IEnumerable<TResult> Select<T, TResult>(this IEnumerable<T> source,
                                                          Delegate<TResult, T> selector)
    {
        foreach (T item in source)
        {
            TResult result = selector(item);
            yield return result;
        }
    }

    // The result of the selector for each item, that has the index of the item too.
    public static IEnumerable<TResult> Select<T, TResult>(this IEnumerable<T> source,
                                                          Delegate<TResult, T, Int32> selector)
    {
        Int32 index = 0;
        foreach (T item in source)
        {
            TResult result = selector(item, index);
            yield return result;
            index++;
        }
    }

    // The items of the collections that the selector gives for each item.
    public static IEnumerable<TResult> SelectMany<T, TResult>(this IEnumerable<T> source,
                                                Delegate<IEnumerable<TResult>, T> selector)
    {
        foreach (T item in source)
        {
            IEnumerable<TResult> inner = selector(item);
            foreach (TResult result in inner)
                yield return result;
        }
    }

    // The items of the collections that the selector gives for each item, and the result of the
    // result selector for each of them.
    public static IEnumerable<TResult> SelectMany<T, TCollection, TResult>(
        this IEnumerable<T> source, Delegate<IEnumerable<TCollection>, T> collectionSelector,
        Delegate<TResult, T, TCollection> resultSelector)
    {
        foreach (T item in source)
        {
            IEnumerable<TCollection> inner = collectionSelector(item);
            foreach (TCollection element in inner)
            {
                TResult result = resultSelector(item, element);
                yield return result;
            }
        }
    }

    // The result of the selector for the items of the two collections that are at the same
    // place: it ends when one of them ends.
    public static IEnumerable<TResult> Zip<T, TOther, TResult>(this IEnumerable<T> first,
        IEnumerable<TOther> second, Delegate<TResult, T, TOther> resultSelector)
    {
        IEnumerator<TOther> others = second.GetEnumerator();

        foreach (T item in first)
        {
            if (!others.MoveNext())
                yield break;

            TOther other = others.Current;
            TResult result = resultSelector(item, other);
            yield return result;
        }
    }
}
