/*
 *  System.dll
 *
 *  System.Linq: the joins of two collections, and the pair that the query expressions use.
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

// Two values that stay together: the range variables of a query expression that has more than one
// (from x in xs from y in ys ...), see the lq language.
public sealed class TransparentIdentifier<T1, T2>
{
    public TransparentIdentifier(T1 item1, T2 item2)
    {
        __item1 = item1;
        __item2 = item2;
    }

    public T1 Item1 => __item1;

    public T2 Item2 => __item2;

    private T1 __item1;
    private T2 __item2;
}

public static class TransparentIdentifier
{
    // Makes the pair of two values, the types of them are those of the arguments.
    public static TransparentIdentifier<T1, T2> Create<T1, T2>(T1 item1, T2 item2)
        => new TransparentIdentifier<T1, T2>(item1, item2);

    // Calls the function with the values: the query expressions give the names of their range
    // variables to the params of the function, whatever the pair that has them is.
    public static R Bind<R, TArgs ...>(Delegate<R, TArgs ...> function, TArgs ... args)
        => function(args ...);
}

////////// ////////// ////////// ////////// //////////

public static partial class Enumerable
{
    // The result of the result selector for each pair of items that have the same key.
    public static IEnumerable<TResult> Join<T, TInner, TKey, TResult>(this IEnumerable<T> outer,
        IEnumerable<TInner> inner, Delegate<TKey, T> outerKeySelector,
        Delegate<TKey, TInner> innerKeySelector, Delegate<TResult, T, TInner> resultSelector,
        IEqualityComparer<TKey> comparer)
    {
        ILookup<TKey, TInner> lookup = ToLookup<TInner, TKey>(inner, innerKeySelector, comparer);

        foreach (T item in outer)
        {
            TKey key = outerKeySelector(item);
            foreach (TInner match in lookup[key])
            {
                TResult result = resultSelector(item, match);
                yield return result;
            }
        }
    }

    public static IEnumerable<TResult> Join<T, TInner, TKey, TResult>(this IEnumerable<T> outer,
        IEnumerable<TInner> inner, Delegate<TKey, T> outerKeySelector,
        Delegate<TKey, TInner> innerKeySelector, Delegate<TResult, T, TInner> resultSelector)
        => Join<T, TInner, TKey, TResult>(outer, inner, outerKeySelector, innerKeySelector,
                                          resultSelector, EqualityComparer<TKey>.Default);

    // The result of the result selector for each item, and the items that have its key.
    public static IEnumerable<TResult> GroupJoin<T, TInner, TKey, TResult>(
        this IEnumerable<T> outer, IEnumerable<TInner> inner, Delegate<TKey, T> outerKeySelector,
        Delegate<TKey, TInner> innerKeySelector,
        Delegate<TResult, T, IEnumerable<TInner>> resultSelector, IEqualityComparer<TKey> comparer)
    {
        ILookup<TKey, TInner> lookup = ToLookup<TInner, TKey>(inner, innerKeySelector, comparer);

        foreach (T item in outer)
        {
            TKey key = outerKeySelector(item);
            TResult result = resultSelector(item, lookup[key]);
            yield return result;
        }
    }

    public static IEnumerable<TResult> GroupJoin<T, TInner, TKey, TResult>(
        this IEnumerable<T> outer, IEnumerable<TInner> inner, Delegate<TKey, T> outerKeySelector,
        Delegate<TKey, TInner> innerKeySelector,
        Delegate<TResult, T, IEnumerable<TInner>> resultSelector)
        => GroupJoin<T, TInner, TKey, TResult>(outer, inner, outerKeySelector, innerKeySelector,
                                               resultSelector, EqualityComparer<TKey>.Default);
}
