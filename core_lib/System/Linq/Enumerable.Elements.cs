/*
 *  System.dll
 *
 *  System.Linq.Enumerable: the counting and the elements of a collection.
 *
 *  A type has no default equality here: the methods that compare the items take the comparer.
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

public static partial class Enumerable
{
    // The number of the items.
    public static Int32 Count<T>(this IEnumerable<T> source)
    {
        Int32 count = 0;
        foreach (T item in source)
            count++;

        return count;
    }

    // The number of the items that the predicate is true for.
    public static Int32 Count<T>(this IEnumerable<T> source, Delegate<Boolean, T> predicate)
    {
        Int32 count = 0;
        foreach (T item in source)
        {
            if (predicate(item))
                count++;
        }

        return count;
    }

    // Whether the source has any item.
    public static Boolean Any<T>(this IEnumerable<T> source)
    {
        foreach (T item in source)
            return true;

        return false;
    }

    // Whether the predicate is true for any item.
    public static Boolean Any<T>(this IEnumerable<T> source, Delegate<Boolean, T> predicate)
    {
        foreach (T item in source)
        {
            if (predicate(item))
                return true;
        }

        return false;
    }

    // Whether the predicate is true for all the items.
    public static Boolean All<T>(this IEnumerable<T> source, Delegate<Boolean, T> predicate)
    {
        foreach (T item in source)
        {
            if (!predicate(item))
                return false;
        }

        return true;
    }

    // Whether the source has an item that equals the value.
    public static Boolean Contains<T>(this IEnumerable<T> source, T value, IEqualityComparer<T> comparer)
    {
        foreach (T item in source)
        {
            if (comparer.Equals(item, value))
                return true;
        }

        return false;
    }

    // Whether the sources have the same items, in the same order.
    public static Boolean SequenceEqual<T>(this IEnumerable<T> first, IEnumerable<T> second,
                                           IEqualityComparer<T> comparer)
    {
        IEnumerator<T> others = second.GetEnumerator();

        foreach (T item in first)
        {
            if (!others.MoveNext())
                return false;

            T other = others.Current;
            if (!comparer.Equals(item, other))
                return false;
        }

        return !others.MoveNext();
    }

    ////////// ////////// ////////// ////////// //////////
    // First, Last, Single, ElementAt

    // The first item, InvalidOperationException if there is no one.
    public static T First<T>(this IEnumerable<T> source)
    {
        foreach (T item in source)
            return item;

        throw new InvalidOperationException("The sequence has no items.");
    }

    // The first item that the predicate is true for.
    public static T First<T>(this IEnumerable<T> source, Delegate<Boolean, T> predicate)
    {
        foreach (T item in source)
        {
            if (predicate(item))
                return item;
        }

        throw new InvalidOperationException("The sequence has no item that matches.");
    }

    // The first item, the default value if there is no one.
    public static T FirstOrDefault<T>(this IEnumerable<T> source)
    {
        foreach (T item in source)
            return item;

        return default(T);
    }

    // The first item that the predicate is true for, the default value if there is no one.
    public static T FirstOrDefault<T>(this IEnumerable<T> source, Delegate<Boolean, T> predicate)
    {
        foreach (T item in source)
        {
            if (predicate(item))
                return item;
        }

        return default(T);
    }

    // The last item, InvalidOperationException if there is no one.
    public static T Last<T>(this IEnumerable<T> source)
    {
        Boolean found = false;
        T last = default(T);

        foreach (T item in source)
        {
            last = item;
            found = true;
        }

        if (!found)
            throw new InvalidOperationException("The sequence has no items.");

        return last;
    }

    // The last item that the predicate is true for.
    public static T Last<T>(this IEnumerable<T> source, Delegate<Boolean, T> predicate)
    {
        Boolean found = false;
        T last = default(T);

        foreach (T item in source)
        {
            if (predicate(item))
            {
                last = item;
                found = true;
            }
        }

        if (!found)
            throw new InvalidOperationException("The sequence has no item that matches.");

        return last;
    }

    // The last item, the default value if there is no one.
    public static T LastOrDefault<T>(this IEnumerable<T> source)
    {
        T last = default(T);
        foreach (T item in source)
            last = item;

        return last;
    }

    // The last item that the predicate is true for, the default value if there is no one.
    public static T LastOrDefault<T>(this IEnumerable<T> source, Delegate<Boolean, T> predicate)
    {
        T last = default(T);
        foreach (T item in source)
        {
            if (predicate(item))
                last = item;
        }

        return last;
    }

    // The only item: InvalidOperationException if there is no one, or more than one.
    public static T Single<T>(this IEnumerable<T> source)
    {
        Boolean found = false;
        T single = default(T);

        foreach (T item in source)
        {
            if (found)
                throw new InvalidOperationException("The sequence has more than one item.");

            single = item;
            found = true;
        }

        if (!found)
            throw new InvalidOperationException("The sequence has no items.");

        return single;
    }

    // The only item that the predicate is true for.
    public static T Single<T>(this IEnumerable<T> source, Delegate<Boolean, T> predicate)
    {
        Boolean found = false;
        T single = default(T);

        foreach (T item in source)
        {
            if (!predicate(item))
                continue;

            if (found)
                throw new InvalidOperationException("The sequence has more than one item that matches.");

            single = item;
            found = true;
        }

        if (!found)
            throw new InvalidOperationException("The sequence has no item that matches.");

        return single;
    }

    // The only item, the default value if there is no one, InvalidOperationException if there
    // is more than one.
    public static T SingleOrDefault<T>(this IEnumerable<T> source)
    {
        Boolean found = false;
        T single = default(T);

        foreach (T item in source)
        {
            if (found)
                throw new InvalidOperationException("The sequence has more than one item.");

            single = item;
            found = true;
        }

        return single;
    }

    // The item at the index, OutOfRangeException if there is no one.
    public static T ElementAt<T>(this IEnumerable<T> source, Int32 index)
    {
        if (index < 0)
            throw new OutOfRangeException();

        Int32 position = 0;
        foreach (T item in source)
        {
            if (position == index)
                return item;

            position++;
        }

        throw new OutOfRangeException();
    }

    // The item at the index, the default value if there is no one.
    public static T ElementAtOrDefault<T>(this IEnumerable<T> source, Int32 index)
    {
        if (index < 0)
            return default(T);

        Int32 position = 0;
        foreach (T item in source)
        {
            if (position == index)
                return item;

            position++;
        }

        return default(T);
    }

    // The items, or the default value when there is no one.
    public static IEnumerable<T> DefaultIfEmpty<T>(this IEnumerable<T> source)
    {
        Boolean any = false;
        foreach (T item in source)
        {
            any = true;
            yield return item;
        }

        if (!any)
            yield return default(T);
    }

    // The items, or the value when there is no one.
    public static IEnumerable<T> DefaultIfEmpty<T>(this IEnumerable<T> source, T defaultValue)
    {
        Boolean any = false;
        foreach (T item in source)
        {
            any = true;
            yield return item;
        }

        if (!any)
            yield return defaultValue;
    }

    // Whether the source has an item that equals the value by the default equality of the type.
    public static Boolean Contains<T>(this IEnumerable<T> source, T value)
        => Contains<T>(source, value, EqualityComparer<T>.Default);

    // Whether the sources have the same items, in the same order.
    public static Boolean SequenceEqual<T>(this IEnumerable<T> first, IEnumerable<T> second)
        => SequenceEqual<T>(first, second, EqualityComparer<T>.Default);
}
