/*
 *  System.dll
 *
 *  System.Linq.Enumerable: the aggregates of a collection: the sums, the extremes, the average.
 *
 *  The overloads of the numbers are told by the type of the items: Int32, Int64 and Double.
 *  The sums of the results of a selector have a name for each type of the result, a lambda
 *  has no type that tells the overload: Sum (Int32), SumInt64, SumDouble.
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

public static partial class Enumerable
{
    ////////// ////////// ////////// ////////// //////////
    // Aggregate

    // Applies the function to the items, the result of it is the first param of the next one.
    public static T Aggregate<T>(this IEnumerable<T> source, Delegate<T, T, T> func)
    {
        Boolean first = true;
        T accumulated = default(T);

        foreach (T item in source)
        {
            if (first)
            {
                accumulated = item;
                first = false;
            }
            else
            {
                accumulated = func(accumulated, item);
            }
        }

        if (first)
            throw new InvalidOperationException("The sequence has no items.");

        return accumulated;
    }

    // Applies the function to the items, from the seed.
    public static TAccumulate Aggregate<T, TAccumulate>(this IEnumerable<T> source, TAccumulate seed,
                                                        Delegate<TAccumulate, TAccumulate, T> func)
    {
        TAccumulate accumulated = seed;
        foreach (T item in source)
            accumulated = func(accumulated, item);

        return accumulated;
    }

    // Applies the function to the items, from the seed, and gives the result of the selector.
    public static TResult Aggregate<T, TAccumulate, TResult>(this IEnumerable<T> source,
        TAccumulate seed, Delegate<TAccumulate, TAccumulate, T> func,
        Delegate<TResult, TAccumulate> resultSelector)
    {
        TAccumulate accumulated = seed;
        foreach (T item in source)
            accumulated = func(accumulated, item);

        return resultSelector(accumulated);
    }

    ////////// ////////// ////////// ////////// //////////
    // Sum

    public static Int32 Sum(this IEnumerable<Int32> source)
    {
        Int32 sum = 0;
        foreach (Int32 item in source)
            sum += item;

        return sum;
    }

    public static Int64 Sum(this IEnumerable<Int64> source)
    {
        Int64 sum = 0;
        foreach (Int64 item in source)
            sum += item;

        return sum;
    }

    public static Double Sum(this IEnumerable<Double> source)
    {
        Double sum = 0;
        foreach (Double item in source)
            sum += item;

        return sum;
    }

    public static Int32 Sum<T>(this IEnumerable<T> source, Delegate<Int32, T> selector)
    {
        Int32 sum = 0;
        foreach (T item in source)
        {
            Int32 value = selector(item);
            sum += value;
        }

        return sum;
    }

    public static Int64 SumInt64<T>(this IEnumerable<T> source, Delegate<Int64, T> selector)
    {
        Int64 sum = 0;
        foreach (T item in source)
        {
            Int64 value = selector(item);
            sum += value;
        }

        return sum;
    }

    public static Double SumDouble<T>(this IEnumerable<T> source, Delegate<Double, T> selector)
    {
        Double sum = 0;
        foreach (T item in source)
        {
            Double value = selector(item);
            sum += value;
        }

        return sum;
    }

    ////////// ////////// ////////// ////////// //////////
    // Average

    public static Double Average(this IEnumerable<Int32> source)
    {
        Int64 sum = 0;
        Int32 count = 0;
        foreach (Int32 item in source)
        {
            sum += item;
            count++;
        }

        if (count == 0)
            throw new InvalidOperationException("The sequence has no items.");

        return (Double)sum / count;
    }

    public static Double Average(this IEnumerable<Int64> source)
    {
        Int64 sum = 0;
        Int32 count = 0;
        foreach (Int64 item in source)
        {
            sum += item;
            count++;
        }

        if (count == 0)
            throw new InvalidOperationException("The sequence has no items.");

        return (Double)sum / count;
    }

    public static Double Average(this IEnumerable<Double> source)
    {
        Double sum = 0;
        Int32 count = 0;
        foreach (Double item in source)
        {
            sum += item;
            count++;
        }

        if (count == 0)
            throw new InvalidOperationException("The sequence has no items.");

        return sum / count;
    }

    // The average of the results of the selector (the numbers are converted to Double).
    public static Double Average<T>(this IEnumerable<T> source, Delegate<Double, T> selector)
    {
        Double sum = 0;
        Int32 count = 0;
        foreach (T item in source)
        {
            Double value = selector(item);
            sum += value;
            count++;
        }

        if (count == 0)
            throw new InvalidOperationException("The sequence has no items.");

        return sum / count;
    }

    ////////// ////////// ////////// ////////// //////////
    // Min, Max

    public static Int32 Min(this IEnumerable<Int32> source)
    {
        Boolean first = true;
        Int32 min = 0;
        foreach (Int32 item in source)
        {
            if (first || item < min)
                min = item;

            first = false;
        }

        if (first)
            throw new InvalidOperationException("The sequence has no items.");

        return min;
    }

    public static Int64 Min(this IEnumerable<Int64> source)
    {
        Boolean first = true;
        Int64 min = 0;
        foreach (Int64 item in source)
        {
            if (first || item < min)
                min = item;

            first = false;
        }

        if (first)
            throw new InvalidOperationException("The sequence has no items.");

        return min;
    }

    public static Double Min(this IEnumerable<Double> source)
    {
        Boolean first = true;
        Double min = 0;
        foreach (Double item in source)
        {
            if (first || item < min)
                min = item;

            first = false;
        }

        if (first)
            throw new InvalidOperationException("The sequence has no items.");

        return min;
    }

    public static Int32 Max(this IEnumerable<Int32> source)
    {
        Boolean first = true;
        Int32 max = 0;
        foreach (Int32 item in source)
        {
            if (first || item > max)
                max = item;

            first = false;
        }

        if (first)
            throw new InvalidOperationException("The sequence has no items.");

        return max;
    }

    public static Int64 Max(this IEnumerable<Int64> source)
    {
        Boolean first = true;
        Int64 max = 0;
        foreach (Int64 item in source)
        {
            if (first || item > max)
                max = item;

            first = false;
        }

        if (first)
            throw new InvalidOperationException("The sequence has no items.");

        return max;
    }

    public static Double Max(this IEnumerable<Double> source)
    {
        Boolean first = true;
        Double max = 0;
        foreach (Double item in source)
        {
            if (first || item > max)
                max = item;

            first = false;
        }

        if (first)
            throw new InvalidOperationException("The sequence has no items.");

        return max;
    }

    // The least item by the comparer.
    public static T Min<T>(this IEnumerable<T> source, IComparer<T> comparer)
    {
        Boolean first = true;
        T min = default(T);
        foreach (T item in source)
        {
            if (first || comparer.Compare(item, min) < 0)
                min = item;

            first = false;
        }

        if (first)
            throw new InvalidOperationException("The sequence has no items.");

        return min;
    }

    // The greatest item by the comparer.
    public static T Max<T>(this IEnumerable<T> source, IComparer<T> comparer)
    {
        Boolean first = true;
        T max = default(T);
        foreach (T item in source)
        {
            if (first || comparer.Compare(item, max) > 0)
                max = item;

            first = false;
        }

        if (first)
            throw new InvalidOperationException("The sequence has no items.");

        return max;
    }

    // The least result of the selector by the comparer.
    public static TResult Min<T, TResult>(this IEnumerable<T> source, Delegate<TResult, T> selector,
                                          IComparer<TResult> comparer)
    {
        Boolean first = true;
        TResult min = default(TResult);
        foreach (T item in source)
        {
            TResult value = selector(item);
            if (first || comparer.Compare(value, min) < 0)
                min = value;

            first = false;
        }

        if (first)
            throw new InvalidOperationException("The sequence has no items.");

        return min;
    }

    // The greatest result of the selector by the comparer.
    public static TResult Max<T, TResult>(this IEnumerable<T> source, Delegate<TResult, T> selector,
                                          IComparer<TResult> comparer)
    {
        Boolean first = true;
        TResult max = default(TResult);
        foreach (T item in source)
        {
            TResult value = selector(item);
            if (first || comparer.Compare(value, max) > 0)
                max = value;

            first = false;
        }

        if (first)
            throw new InvalidOperationException("The sequence has no items.");

        return max;
    }

    // The least item by the default order of the type.
    public static T Min<T>(this IEnumerable<T> source)
        => Min<T>(source, Comparer<T>.Default);

    // The greatest item by the default order of the type.
    public static T Max<T>(this IEnumerable<T> source)
        => Max<T>(source, Comparer<T>.Default);

    // The least result of the selector by the default order of the type.
    public static TResult Min<T, TResult>(this IEnumerable<T> source, Delegate<TResult, T> selector)
        => Min<T, TResult>(source, selector, Comparer<TResult>.Default);

    // The greatest result of the selector by the default order of the type.
    public static TResult Max<T, TResult>(this IEnumerable<T> source, Delegate<TResult, T> selector)
        => Max<T, TResult>(source, selector, Comparer<TResult>.Default);
}
