/*
 *  System.dll
 *
 *  Index and Range: a[^1] is the last item, a[1..3] the items from the second to the third. The compiler writes the calls of
 *  Index.FromEnd, Range.Of, and RangeHelper.At and RangeHelper.Slice, that take the item and the items of an array, a string
 *  and a List.
 */

using Collections;

////////// ////////// ////////// ////////// //////////

/// <summary>A position of the items: counted from the start (3), or from the end (^3).</summary>
public struct Index : IStringable
{
    private Int32 __value;
    private Boolean __fromEnd;

    public Index(Int32 value, Boolean fromEnd)
    {
        if (value < 0)
            throw new ArgumentException("The index is not negative.");

        __value = value;
        __fromEnd = fromEnd;
    }

    /// <summary>The position counted from the end: the last item is 1.</summary>
    public static Index FromEnd(Int32 value) => new Index(value, true);

    /// <summary>The position counted from the start.</summary>
    public static Index FromStart(Int32 value) => new Index(value, false);

    public Int32 Value => __value;

    public Boolean IsFromEnd => __fromEnd;

    /// <summary>The offset of the position in a list of the length.</summary>
    public Int32 GetOffset(Int32 length) => __fromEnd? length - __value : __value;

    public static implicit operator Index(Int32 value) => new Index(value, false);

    public String ToString() => (__fromEnd? "^" : "") + __value;
}

////////// ////////// ////////// ////////// //////////

/// <summary>The items between two positions: the start is in the range, the end is not.</summary>
public struct Range : IStringable
{
    private Index __start;
    private Index __end;

    public Range(Index start, Index end)
    {
        __start = start;
        __end = end;
    }

    /// <summary>1..3 is Of(1, false, 3, false); ..2 is Of(0, false, 2, false); 1.. is Of(1, false, 0, true).</summary>
    public static Range Of(Int32 start, Boolean startFromEnd, Int32 end, Boolean endFromEnd)
        => new Range(new Index(start, startFromEnd), new Index(end, endFromEnd));

    public Index Start => __start;

    public Index End => __end;

    /// <summary>The offset of the first item, and the count of the items, in a list of the length.</summary>
    /// <exception cref="OutOfRangeException">The range is not in the list.</exception>
    public Void GetOffsetAndLength(Int32 length, out Int32 offset, out Int32 count)
    {
        Int32 start = __start.GetOffset(length);
        Int32 end = __end.GetOffset(length);

        if (start < 0 || end > length || start > end)
            throw new OutOfRangeException();

        offset = start;
        count = end - start;
    }

    public String ToString() => __start.ToString() + ".." + __end.ToString();
}

////////// ////////// ////////// ////////// //////////

/// <summary>The items of a list that an index and a range tell (a[^1], a[1..3]).</summary>
public static class RangeHelper
{
    // ^1 is FromEnd(1), 1..3 is Of(1, false, 3, false): the compiler writes these calls (a type of the code can have the name Index, or Range).
    public static Index FromEnd(Int32 value) => new Index(value, true);

    public static Range Of(Int32 start, Boolean startFromEnd, Int32 end, Boolean endFromEnd)
        => new Range(new Index(start, startFromEnd), new Index(end, endFromEnd));

    public static T At<T>(T[] array, Index index)
    {
        Int32 offset = index.GetOffset(array.Length);
        return array[offset];
    }

    public static Char At(String text, Index index)
    {
        Int32 offset = index.GetOffset(text.Length);
        return text[offset];
    }

    public static T At<T>(List<T> list, Index index)
    {
        Int32 offset = index.GetOffset(list.Count);
        return list[offset];
    }

    public static T[] Slice<T>(T[] array, Range range)
    {
        Int32 offset, count;
        range.GetOffsetAndLength(array.Length, out offset, out count);

        T[] result = new T[count];
        for (Int32 k = 0; k < count; k++)
            result[k] = array[offset + k];

        return result;
    }

    public static String Slice(String text, Range range)
    {
        Int32 offset, count;
        range.GetOffsetAndLength(text.Length, out offset, out count);

        return text.Substring(offset, count);
    }

    public static List<T> Slice<T>(List<T> list, Range range)
    {
        Int32 offset, count;
        range.GetOffsetAndLength(list.Count, out offset, out count);

        List<T> result = new List<T>();
        for (Int32 k = 0; k < count; k++)
            result.Add(list[offset + k]);

        return result;
    }
}
