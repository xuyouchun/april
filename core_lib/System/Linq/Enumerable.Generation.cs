/*
 *  System.dll
 *
 *  System.Linq.Enumerable: the methods that make a collection.
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

public static partial class Enumerable
{
    // The numbers from the start, count of them.
    public static IEnumerable<Int32> Range(Int32 start, Int32 count)
    {
        if (count < 0)
            throw new OutOfRangeException();

        for (Int32 index = 0; index < count; index++)
            yield return start + index;
    }

    // The value, count times.
    public static IEnumerable<T> Repeat<T>(T value, Int32 count)
    {
        if (count < 0)
            throw new OutOfRangeException();

        for (Int32 index = 0; index < count; index++)
            yield return value;
    }

    // A collection that has no item.
    public static IEnumerable<T> Empty<T>() => new List<T>();
}
