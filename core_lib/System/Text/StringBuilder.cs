/*
 *  System.dll
 *
 *  StringBuilder: builds a string from parts without making a string for each part.
 */

namespace Text;

////////// ////////// ////////// ////////// //////////

// A string that can be changed: the characters are kept in an array that grows when it is full,
// so that appending is done in a time that does not depend on the length of the text.
public class StringBuilder : IStringable
{
    public StringBuilder()
    {
        __chars = new Char[16];
        __length = 0;
    }

    // Creates a builder with room for the specified number of characters.
    public StringBuilder(Int32 capacity)
    {
        if (capacity < 0)
            throw new OutOfRangeException();

        __chars = new Char[capacity < 16? 16 : capacity];
        __length = 0;
    }

    // Creates a builder that has the characters of the string.
    public StringBuilder(String? value)
    {
        __chars = new Char[16];
        __length = 0;
        Append(value);
    }

    // The number of characters that the builder holds without making a larger array.
    public Int32 Capacity => __chars.Length;

    // The number of characters. A larger value fills the end with the character 0, a smaller one cuts the text.
    public Int32 Length
    {
        get => __length;

        set
        {
            if (value < 0)
                throw new OutOfRangeException();

            EnsureCapacity(value);

            for (Int32 k = __length; k < value; k++)
                __chars[k] = '\0';

            __length = value;
        }
    }

    // The character at the index.
    public Char this[Int32 index]
    {
        get
        {
            if (index < 0 || index >= __length)
                throw new OutOfRangeException();

            return __chars[index];
        }

        set
        {
            if (index < 0 || index >= __length)
                throw new OutOfRangeException();

            __chars[index] = value;
        }
    }

    // Makes the capacity at least the specified number of characters; returns the capacity.
    public Int32 EnsureCapacity(Int32 capacity)
    {
        if (capacity < 0)
            throw new OutOfRangeException();

        if (capacity > __chars.Length)
        {
            Int32 size = __chars.Length * 2;
            if (size < capacity)
                size = capacity;

            Char[] chars = new Char[size];
            for (Int32 k = 0; k < __length; k++)
                chars[k] = __chars[k];

            __chars = chars;
        }

        return __chars.Length;
    }

    // Removes all the characters.
    public StringBuilder Clear()
    {
        __length = 0;
        return this;
    }

    // Appends the text of a value. A null string or object appends nothing.
    public StringBuilder Append(String? value)
    {
        if (value == null)
            return this;

        Int32 count = value.Length;
        EnsureCapacity(__length + count);
        Internal.String_CopyTo(value, 0, __chars, __length, count);
        __length += count;
        return this;
    }

    // Appends the characters of the string from the start.
    public StringBuilder Append(String value, Int32 start, Int32 count)
    {
        if (start < 0 || count < 0 || start > value.Length - count)
            throw new OutOfRangeException();

        EnsureCapacity(__length + count);
        Internal.String_CopyTo(value, start, __chars, __length, count);
        __length += count;
        return this;
    }

    public StringBuilder Append(Char value)
    {
        EnsureCapacity(__length + 1);
        __chars[__length++] = value;
        return this;
    }

    // Appends the character the number of times.
    public StringBuilder Append(Char value, Int32 repeat)
    {
        if (repeat < 0)
            throw new OutOfRangeException();

        EnsureCapacity(__length + repeat);
        for (Int32 k = 0; k < repeat; k++)
            __chars[__length++] = value;

        return this;
    }

    public StringBuilder Append(Char[] values)
    {
        EnsureCapacity(__length + values.Length);
        for (Int32 k = 0; k < values.Length; k++)
            __chars[__length++] = values[k];

        return this;
    }

    // Appends the characters of the array from the start.
    public StringBuilder Append(Char[] values, Int32 start, Int32 count)
    {
        if (start < 0 || count < 0 || start > values.Length - count)
            throw new OutOfRangeException();

        EnsureCapacity(__length + count);
        for (Int32 k = 0; k < count; k++)
            __chars[__length++] = values[start + k];

        return this;
    }

    // Appends the one or two characters of the rune.
    public StringBuilder Append(Rune value) => Append(value.ToString());

    // Appends the text that String.Format makes.
    public StringBuilder AppendFormat(String format, params Object[] args) => Append(String.Format(format, args));

    public StringBuilder AppendFormat(IFormatProvider? provider, String format, params Object[] args)
        => Append(String.Format(provider, format, args));

    // Appends the characters of another builder (also the builder itself).
    public StringBuilder Append(StringBuilder? value)
    {
        if (value == null)
            return this;

        Int32 count = value.__length;
        EnsureCapacity(__length + count);
        for (Int32 k = 0; k < count; k++)
            __chars[__length + k] = value.__chars[k];

        __length += count;
        return this;
    }

    // Appends the values with the separator between them.
    public StringBuilder AppendJoin(String? separator, String[] values)
    {
        for (Int32 k = 0; k < values.Length; k++)
        {
            if (k > 0)
                Append(separator);

            Append(values[k]);
        }

        return this;
    }

    // Appends the text of a value of a type that has one (the numbers, Boolean, and the types that
    // implement IStringable): the value is not boxed.
    public StringBuilder Append<T>(T value) where T : IStringable => Append(value.ToString());

    // Appends the ToString of the object.
    public StringBuilder Append(Object? value)
    {
        if (value == null)
            return this;

        return Append(Object.ToString(value));
    }

    // Appends the end of a line.
    public StringBuilder AppendLine() => Append('\n');

    public StringBuilder AppendLine(String? value) => Append(value).Append('\n');

    public StringBuilder AppendLine<T>(T value) where T : IStringable => Append(value).Append('\n');

    // Inserts the text at the index.
    public StringBuilder Insert(Int32 index, String? value)
    {
        if (index < 0 || index > __length)
            throw new OutOfRangeException();

        if (value == null || value.Length == 0)
            return this;

        Int32 count = value.Length;
        EnsureCapacity(__length + count);

        for (Int32 k = __length - 1; k >= index; k--)
            __chars[k + count] = __chars[k];

        Internal.String_CopyTo(value, 0, __chars, index, count);
        __length += count;
        return this;
    }

    // Inserts the text the number of times.
    public StringBuilder Insert(Int32 index, String? value, Int32 repeat)
    {
        if (repeat < 0)
            throw new OutOfRangeException();

        if (value == null || repeat == 0)
            return Insert(index, (String?)null);

        StringBuilder parts = new StringBuilder(value.Length * repeat);
        for (Int32 k = 0; k < repeat; k++)
            parts.Append(value);

        return Insert(index, parts.ToString());
    }

    public StringBuilder Insert(Int32 index, Char[] values) => Insert(index, String.FromChars(values));

    public StringBuilder Insert(Int32 index, Char value) => Insert(index, value.ToString());

    public StringBuilder Insert<T>(Int32 index, T value) where T : IStringable => Insert(index, value.ToString());

    // Removes the characters of the count from the index.
    public StringBuilder Remove(Int32 index, Int32 count)
    {
        if (index < 0 || count < 0 || index > __length - count)
            throw new OutOfRangeException();

        for (Int32 k = index + count; k < __length; k++)
            __chars[k - count] = __chars[k];

        __length -= count;
        return this;
    }

    // Replaces each old value by the new one.
    public StringBuilder Replace(String oldValue, String newValue)
    {
        String text = ToString().Replace(oldValue, newValue);

        __length = 0;
        return Append(text);
    }

    public StringBuilder Replace(Char oldValue, Char newValue)
    {
        for (Int32 k = 0; k < __length; k++)
        {
            if (__chars[k] == oldValue)
                __chars[k] = newValue;
        }

        return this;
    }

    // Replaces each old value in the characters of the count from the start.
    public StringBuilder Replace(String oldValue, String newValue, Int32 start, Int32 count)
    {
        if (start < 0 || count < 0 || start > __length - count)
            throw new OutOfRangeException();

        String part = ToString(start, count).Replace(oldValue, newValue);
        Remove(start, count);
        return Insert(start, part);
    }

    public StringBuilder Replace(Char oldValue, Char newValue, Int32 start, Int32 count)
    {
        if (start < 0 || count < 0 || start > __length - count)
            throw new OutOfRangeException();

        for (Int32 k = start; k < start + count; k++)
        {
            if (__chars[k] == oldValue)
                __chars[k] = newValue;
        }

        return this;
    }

    // Copies the characters of the count from the start to the array at the index.
    public Void CopyTo(Int32 start, Char[] destination, Int32 index, Int32 count)
    {
        if (start < 0 || count < 0 || start > __length - count || index < 0 || index > destination.Length - count)
            throw new OutOfRangeException();

        for (Int32 k = 0; k < count; k++)
            destination[index + k] = __chars[start + k];
    }

    // Whether the other builder has the same characters.
    public Boolean Equals(StringBuilder? other)
    {
        if (other == null || other.__length != __length)
            return false;

        for (Int32 k = 0; k < __length; k++)
        {
            if (__chars[k] != other.__chars[k])
                return false;
        }

        return true;
    }

    // The text of the builder.
    public String ToString() => Internal.String_FromChars(__chars, 0, __length);

    // The text of the characters of the length from the start.
    public String ToString(Int32 start, Int32 length)
    {
        if (start < 0 || length < 0 || start > __length - length)
            throw new OutOfRangeException();

        return Internal.String_FromChars(__chars, start, length);
    }

    private Char[] __chars;

    private Int32 __length;
}
