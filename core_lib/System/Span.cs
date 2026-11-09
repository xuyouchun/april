/*
 *  System.dll
 *
 *  Span<T> and ReadOnlySpan<T>: a window on the items of an array.
 */

////////// ////////// ////////// ////////// //////////

// A range of the items of an array, that is not a copy of them: a change of an item of the span is a change of the array,
// and a slice of the span is a window on the same items. The span keeps the array alive. A span is a value (3 fields):
// it is cheap to pass, and to slice.
public struct Span<T>
{
    // The span of all the items of the array; a null array is an empty span.
    public Span(T[] array)
    {
        __array = array;
        __start = 0;
        __length = array == null ? 0 : array.Length;
    }

    // The span of the items of the length from the start of the array.
    public Span(T[] array, Int32 start, Int32 length)
    {
        if (array == null)
        {
            if (start != 0 || length != 0)
                throw new OutOfRangeException();
        }
        else if (start < 0 || length < 0 || start > array.Length - length)
        {
            throw new OutOfRangeException();
        }

        __array = array;
        __start = start;
        __length = length;
    }

    // The span that has no items.
    public static Span<T> Empty => new Span<T>(default(T[]));

    // The array, and the index of the first item: for the spans that copy from or to this one.
    internal T[] Items => __array;

    internal Int32 Offset => __start;

    // The number of the items.
    public Int32 Length => __length;

    public Boolean IsEmpty => __length == 0;

    // The item at the index: an OutOfRangeException when the index is not in the span.
    public T this[Int32 index]
    {
        get
        {
            if (index < 0 || index >= __length)
                throw new OutOfRangeException();

            return __array[__start + index];
        }

        set
        {
            if (index < 0 || index >= __length)
                throw new OutOfRangeException();

            __array[__start + index] = value;
        }
    }

    // The span of the items from the start to the end of this one.
    public Span<T> Slice(Int32 start) => Slice(start, __length - start);

    // The span of the items of the length from the start.
    public Span<T> Slice(Int32 start, Int32 length)
    {
        if (start < 0 || length < 0 || start > __length - length)
            throw new OutOfRangeException();

        return new Span<T>(__array, __start + start, length);
    }

    // A new array with the items.
    public T[] ToArray()
    {
        T[] items = new T[__length];

        for (Int32 k = 0; k < __length; k++)
            items[k] = __array[__start + k];

        return items;
    }

    // Copies the items to the destination, that must have room for all of them (an OutOfRangeException).
    // The spans can overlap: the items are the ones that the source had.
    public Void CopyTo(Span<T> destination)
    {
        if (destination.Length < __length)
            throw new OutOfRangeException();

        Span<T>.__Copy(__array, __start, destination.Items, destination.Offset, __length);
    }

    // Copies the items when the destination has room for them; returns whether it did.
    public Boolean TryCopyTo(Span<T> destination)
    {
        if (destination.Length < __length)
            return false;

        Span<T>.__Copy(__array, __start, destination.Items, destination.Offset, __length);
        return true;
    }

    // Sets all the items to the value.
    public Void Fill(T value)
    {
        for (Int32 k = 0; k < __length; k++)
            __array[__start + k] = value;
    }

    // Sets all the items to the default value of the type.
    public Void Clear()
    {
        T zero = default(T);

        for (Int32 k = 0; k < __length; k++)
            __array[__start + k] = zero;
    }

    // The items, in the order, for foreach.
    public SpanEnumerator<T> GetEnumerator() => new SpanEnumerator<T>(__array, __start, __length);

    public static implicit operator Span<T>(T[] array) => new Span<T>(array);

    public static implicit operator ReadOnlySpan<T>(Span<T> span) => new ReadOnlySpan<T>(span.Items, span.Offset, span.Length);

    // Copies the items of a range, the ranges can overlap.
    internal static Void __Copy(T[] source, Int32 sourceStart, T[] destination, Int32 destinationStart, Int32 count)
    {
        if (Object.ReferenceEquals(source, destination) && destinationStart > sourceStart)
        {
            for (Int32 k = count - 1; k >= 0; k--)
                destination[destinationStart + k] = source[sourceStart + k];
        }
        else
        {
            for (Int32 k = 0; k < count; k++)
                destination[destinationStart + k] = source[sourceStart + k];
        }
    }

    private T[] __array;

    private Int32 __start;

    private Int32 __length;
}

////////// ////////// ////////// ////////// //////////

// A span whose items cannot be changed by it (the array can be changed by others).
public struct ReadOnlySpan<T>
{
    public ReadOnlySpan(T[] array)
    {
        __array = array;
        __start = 0;
        __length = array == null ? 0 : array.Length;
    }

    public ReadOnlySpan(T[] array, Int32 start, Int32 length)
    {
        if (array == null)
        {
            if (start != 0 || length != 0)
                throw new OutOfRangeException();
        }
        else if (start < 0 || length < 0 || start > array.Length - length)
        {
            throw new OutOfRangeException();
        }

        __array = array;
        __start = start;
        __length = length;
    }

    public static ReadOnlySpan<T> Empty => new ReadOnlySpan<T>(default(T[]));

    internal T[] Items => __array;

    internal Int32 Offset => __start;

    public Int32 Length => __length;

    public Boolean IsEmpty => __length == 0;

    public T this[Int32 index]
    {
        get
        {
            if (index < 0 || index >= __length)
                throw new OutOfRangeException();

            return __array[__start + index];
        }
    }

    public ReadOnlySpan<T> Slice(Int32 start) => Slice(start, __length - start);

    public ReadOnlySpan<T> Slice(Int32 start, Int32 length)
    {
        if (start < 0 || length < 0 || start > __length - length)
            throw new OutOfRangeException();

        return new ReadOnlySpan<T>(__array, __start + start, length);
    }

    public T[] ToArray()
    {
        T[] items = new T[__length];

        for (Int32 k = 0; k < __length; k++)
            items[k] = __array[__start + k];

        return items;
    }

    public Void CopyTo(Span<T> destination)
    {
        if (destination.Length < __length)
            throw new OutOfRangeException();

        Span<T>.__Copy(__array, __start, destination.Items, destination.Offset, __length);
    }

    public Boolean TryCopyTo(Span<T> destination)
    {
        if (destination.Length < __length)
            return false;

        Span<T>.__Copy(__array, __start, destination.Items, destination.Offset, __length);
        return true;
    }

    public SpanEnumerator<T> GetEnumerator() => new SpanEnumerator<T>(__array, __start, __length);

    public static implicit operator ReadOnlySpan<T>(T[] array) => new ReadOnlySpan<T>(array);

    private T[] __array;

    private Int32 __start;

    private Int32 __length;
}

////////// ////////// ////////// ////////// //////////

// The enumerator of the items of a span (foreach): a value, the items are not copied.
public struct SpanEnumerator<T>
{
    public SpanEnumerator(T[] array, Int32 start, Int32 length)
    {
        __array = array;
        __next = start;
        __end = start + length;
    }

    // Goes to the next item; returns false at the end.
    public Boolean MoveNext()
    {
        if (__next >= __end)
            return false;

        __current = __array[__next];
        __next++;
        return true;
    }

    public T Current => __current;

    private T[] __array;

    private Int32 __next;

    private Int32 __end;

    private T __current;
}
