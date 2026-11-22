
/*
 *  System.dll
 *
 *  String: the members of the strings (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct String
{
    public Int32 CompareTo(String other) => Internal.String_Compare(this, other);

    public Boolean Equals(String other) => Internal.String_Equals(this, other);

    public Boolean Equals(Object? obj) => obj is String && Internal.String_Equals(this, (String)obj);

    public Int32 GetHashCode() => Internal.String_GetHashCode(this);

    public Int32 Length => Internal.String_GetLength(this);

    // The characters from the start to the end of the string.
    public String Substring(Int32 start) => Substring(start, Length - start);

    // The characters of the length from the start, the range must be in the string.
    public String Substring(Int32 start, Int32 length)
    {
        if (start < 0 || length < 0 || start > Length - length)
            throw new OutOfRangeException();

        return Internal.String_Substring(this, start, length);
    }

    public String ToString() => this;

    // The empty string.
    public static String Empty => "";

    // The character at the index, the index must be in the string.
    public Char this[Int32 index]
    {
        get
        {
            if (index < 0 || index >= Length)
                throw new OutOfRangeException();

            return Internal.String_GetChar(this, index);
        }
    }

    public static Boolean IsNullOrEmpty(String? value) => value == null || value.Length == 0;

    // Whether the string has no characters, or only white spaces.
    public static Boolean IsNullOrWhiteSpace(String? value)
    {
        if (value == null)
            return true;

        return Internal.String_IsBlank(value);
    }

    // The first position of the character or of the string, or -1.
    public Int32 IndexOf(Char value) => IndexOf(value, 0);

    public Int32 IndexOf(Char value, Int32 start)
    {
        if (start < 0 || start > Length)
            throw new OutOfRangeException();

        return Internal.String_IndexOfChar(this, value, start);
    }

    public Int32 IndexOf(String value) => IndexOf(value, 0);

    public Int32 IndexOf(String value, Int32 start)
    {
        if (start < 0 || start > Length)
            throw new OutOfRangeException();

        return Internal.String_IndexOf(this, value, start);
    }

    // The last position of the character or of the string, or -1.
    public Int32 LastIndexOf(Char value) => Internal.String_LastIndexOfChar(this, value);

    public Int32 LastIndexOf(String value) => Internal.String_LastIndexOf(this, value, Length);

    public Boolean Contains(Char value) => IndexOf(value) >= 0;

    public Boolean Contains(String value) => Internal.String_IndexOf(this, value, 0) >= 0;

    public Boolean StartsWith(Char value) => Length > 0 && Internal.String_GetChar(this, 0) == value;

    public Boolean StartsWith(String value)
    {
        return value.Length <= Length && Internal.String_Equals(Internal.String_Substring(this, 0, value.Length), value);
    }

    public Boolean EndsWith(Char value) => Length > 0 && Internal.String_GetChar(this, Length - 1) == value;

    public Boolean EndsWith(String value)
    {
        return value.Length <= Length
            && Internal.String_Equals(Internal.String_Substring(this, Length - value.Length, value.Length), value);
    }

    public String ToUpper() => Internal.String_ToUpper(this);

    public String ToLower() => Internal.String_ToLower(this);

    // Without the white spaces at the start and at the end.
    public String Trim() => Internal.String_Trim(this, 3);

    public String TrimStart() => Internal.String_Trim(this, 1);

    public String TrimEnd() => Internal.String_Trim(this, 2);

    // The string padded with the character at the start (PadLeft) or at the end (PadRight) to the width.
    public String PadLeft(Int32 width) => PadLeft(width, ' ');

    public String PadLeft(Int32 width, Char c)
    {
        if (Length >= width)
            return this;

        return new Text.StringBuilder(width).Append(c, width - Length).Append(this).ToString();
    }

    public String PadRight(Int32 width) => PadRight(width, ' ');

    public String PadRight(Int32 width, Char c)
    {
        if (Length >= width)
            return this;

        return new Text.StringBuilder(width).Append(this).Append(c, width - Length).ToString();
    }

    // The string with the value at the index.
    public String Insert(Int32 index, String value)
    {
        if (index < 0 || index > Length)
            throw new OutOfRangeException();

        return Internal.String_Substring(this, 0, index) + value
            + Internal.String_Substring(this, index, Length - index);
    }

    // The string without the characters of the count from the index (or to the end).
    public String Remove(Int32 index) => Remove(index, Length - index);

    public String Remove(Int32 index, Int32 count)
    {
        if (index < 0 || count < 0 || index > Length - count)
            throw new OutOfRangeException();

        return Internal.String_Substring(this, 0, index)
            + Internal.String_Substring(this, index + count, Length - index - count);
    }

    // The string with each old value replaced by the new one.
    public String Replace(String oldValue, String newValue)
    {
        if (oldValue.Length == 0)
            throw new OutOfRangeException();

        String result = "";
        Int32 begin = 0;

        for (Int32 k = Internal.String_IndexOf(this, oldValue, 0); k >= 0;
                k = Internal.String_IndexOf(this, oldValue, begin))
        {
            result = result + Internal.String_Substring(this, begin, k - begin) + newValue;
            begin = k + oldValue.Length;
        }

        return result + Internal.String_Substring(this, begin, Length - begin);
    }

    public String Replace(Char oldValue, Char newValue) => Replace(oldValue.ToString(), newValue.ToString());

    // The parts that the separator splits the string into (empty parts are kept).
    public String[] Split(Char separator) => Split(separator.ToString());

    public String[] Split(String separator)
    {
        if (separator.Length == 0)
            throw new OutOfRangeException();

        Int32 count = 1;
        for (Int32 k = Internal.String_IndexOf(this, separator, 0); k >= 0;
                k = Internal.String_IndexOf(this, separator, k + separator.Length))
            count++;

        String[] parts = new String[count];
        Int32 begin = 0, index = 0;

        for (Int32 k = Internal.String_IndexOf(this, separator, 0); k >= 0;
                k = Internal.String_IndexOf(this, separator, begin))
        {
            parts[index++] = Internal.String_Substring(this, begin, k - begin);
            begin = k + separator.Length;
        }

        parts[index] = Internal.String_Substring(this, begin, Length - begin);
        return parts;
    }

    // The characters of the string.
    public Char[] ToCharArray()
    {
        Char[] chars = new Char[Length];
        Internal.String_CopyTo(this, 0, chars, 0, Length);
        return chars;
    }

    // A string of the characters.
    public static String FromChars(Char[] chars)
    {
        return Internal.String_FromChars(chars, 0, chars.Length);
    }

    // The strings with the separator between them.
    public static String Join(String separator, String[] values)
    {
        Text.StringBuilder result = new Text.StringBuilder();
        for (Int32 k = 0; k < values.Length; k++)
        {
            if (k > 0)
                result.Append(separator);

            result.Append(values[k]);
        }

        return result.ToString();
    }

    public static String Concat(String? a, String? b) => Internal.String_Concat(a, b);

    public static String Concat(String? a, String? b, String? c) => Internal.String_Concat3(a, b, c);

    public static String Concat(String? a, String? b, String? c, String? d) => Internal.String_Concat4(a, b, c, d);

    public static String Concat(String? a, String? b, String? c, String? d, String? e) => Internal.String_Concat5(a, b, c, d, e);

    public static String Concat(String? a, String? b, String? c, String? d, String? e, String? f) => Internal.String_Concat6(a, b, c, d, e, f);

    public static String Concat(String? a, String? b, String? c, String? d, String? e, String? f, String? g) => Internal.String_Concat7(a, b, c, d, e, f, g);

    public static String Concat(String? a, String? b, String? c, String? d, String? e, String? f, String? g, String? h) => Internal.String_Concat8(a, b, c, d, e, f, g, h);

    // The text of the format, where {index[,alignment][:format]} is the text of the argument at the index and {{ and }} are
    // the braces. A positive alignment is the width of the text of the argument (it is at the right of it), a negative one
    // has it at the left. The format is given to the arguments that are IFormattable (the numbers: "X2", "F3"), the others
    // have their text. A null argument is nothing. A format that is not valid, or an index that is not in the arguments, is a
    // FormatException.
    public static String Format(String format, params Object[] args) => Format((IFormatProvider?)null, format, args);

    // The same with the symbols of the provider (a NumberFormatInfo) for the numbers.
    public static String Format(IFormatProvider? provider, String format, params Object[] args)
    {
        if (format == null)
            throw new ArgumentException("the format is null");

        Text.StringBuilder result = new Text.StringBuilder(format.Length + 16);
        Int32 length = format.Length, k = 0;

        while (k < length)
        {
            Char c = format[k];

            if (c == '}')
            {
                if (k + 1 < length && format[k + 1] == '}')
                {
                    result.Append('}');
                    k += 2;
                    continue;
                }

                throw new FormatException("the format '" + format + "' has a } that has no {");
            }

            if (c != '{')
            {
                result.Append(c);
                k++;
                continue;
            }

            if (k + 1 < length && format[k + 1] == '{')
            {
                result.Append('{');
                k += 2;
                continue;
            }

            // The index.
            k++;
            Int32 index = 0, digits = 0;
            while (k < length && format[k] >= '0' && format[k] <= '9')
            {
                index = index * 10 + ((Int32)format[k] - (Int32)'0');
                digits++;
                k++;

                if (index > 100000)
                    throw new FormatException("the index of an argument in the format '" + format + "' is too large");
            }

            if (digits == 0)
                throw new FormatException("the format '" + format + "' has a { that is not followed by an index");

            // The alignment.
            Int32 alignment = 0;
            if (k < length && format[k] == ',')
            {
                k++;

                Boolean left = false;
                if (k < length && format[k] == '-')
                {
                    left = true;
                    k++;
                }

                digits = 0;
                while (k < length && format[k] >= '0' && format[k] <= '9')
                {
                    alignment = alignment * 10 + ((Int32)format[k] - (Int32)'0');
                    digits++;
                    k++;

                    if (alignment > 100000)
                        throw new FormatException("the alignment in the format '" + format + "' is too large");
                }

                if (digits == 0)
                    throw new FormatException("the alignment in the format '" + format + "' is not a number");

                if (left)
                    alignment = -alignment;
            }

            // The format of the argument: the text up to the }.
            String? argumentFormat = null;
            if (k < length && format[k] == ':')
            {
                k++;

                Int32 start = k;
                while (k < length && format[k] != '}')
                    k++;

                argumentFormat = format.Substring(start, k - start);
            }

            if (k >= length || format[k] != '}')
                throw new FormatException("the format '" + format + "' has a { that is not closed");

            k++;

            if (args == null || index >= args.Length)
                throw new FormatException("the format '" + format + "' has the index " + index + ", and there are not so many arguments");

            String text = __FormatArgument(args[index], argumentFormat, provider);

            if (alignment > text.Length)
                text = text.PadLeft(alignment, ' ');
            else if (-alignment > text.Length)
                text = text.PadRight(-alignment, ' ');

            result.Append(text);
        }

        return result.ToString();
    }

    // The text of an argument of Format.
    private static String __FormatArgument(Object? argument, String? format, IFormatProvider? provider)
    {
        if (argument == null)
            return "";

        // A string that is null is boxed as an object that is not null: the concatenation takes it as nothing.
        if (argument is String)
            return "" + (String)argument;

        IFormattable? formattable = argument as IFormattable;
        if (formattable != null && ((format != null && format.Length > 0) || provider != null))
            return formattable.ToString(format, provider);

        return Object.ToString(argument);
    }

    // Operators: they are found by the name of the operator.
    public static String operator +(String? a, String? b) => Internal.String_Concat(a, b);

    public static String operator +(String? a, Int8 b) => Internal.String_Concat(a, b.ToString());

    public static String operator +(Int8 a, String? b) => Internal.String_Concat(a.ToString(), b);

    public static String operator +(String? a, UInt8 b) => Internal.String_Concat(a, b.ToString());

    public static String operator +(UInt8 a, String? b) => Internal.String_Concat(a.ToString(), b);

    public static String operator +(String? a, Int16 b) => Internal.String_Concat(a, b.ToString());

    public static String operator +(Int16 a, String? b) => Internal.String_Concat(a.ToString(), b);

    public static String operator +(String? a, UInt16 b) => Internal.String_Concat(a, b.ToString());

    public static String operator +(UInt16 a, String? b) => Internal.String_Concat(a.ToString(), b);

    public static String operator +(String? a, Int32 b) => Internal.String_Concat(a, b.ToString());

    public static String operator +(Int32 a, String? b) => Internal.String_Concat(a.ToString(), b);

    public static String operator +(String? a, UInt32 b) => Internal.String_Concat(a, b.ToString());

    public static String operator +(UInt32 a, String? b) => Internal.String_Concat(a.ToString(), b);

    public static String operator +(String? a, Int64 b) => Internal.String_Concat(a, b.ToString());

    public static String operator +(Int64 a, String? b) => Internal.String_Concat(a.ToString(), b);

    public static String operator +(String? a, UInt64 b) => Internal.String_Concat(a, b.ToString());

    public static String operator +(UInt64 a, String? b) => Internal.String_Concat(a.ToString(), b);

    public static String operator +(String? a, Single b) => Internal.String_Concat(a, b.ToString());

    public static String operator +(Single a, String? b) => Internal.String_Concat(a.ToString(), b);

    public static String operator +(String? a, Double b) => Internal.String_Concat(a, b.ToString());

    public static String operator +(Double a, String? b) => Internal.String_Concat(a.ToString(), b);

    public static String operator +(String? a, Char b) => Internal.String_Concat(a, b.ToString());

    public static String operator +(Char a, String? b) => Internal.String_Concat(a.ToString(), b);

    public static String operator +(String? a, Boolean b)
        => Internal.String_Concat(a, b.ToString());

    public static String operator +(Boolean a, String? b)
        => Internal.String_Concat(a.ToString(), b);

    // An object is converted by its ToString.
    public static String operator +(String? a, Object b)
    {
        if (b == null)
            return Internal.String_Concat(a, null);

        return Internal.String_Concat(a, Object.ToString(b));
    }

    public static String operator +(Object a, String? b)
    {
        if (a == null)
            return Internal.String_Concat(null, b);

        return Internal.String_Concat(Object.ToString(a), b);
    }

    public static Boolean operator ==(String a, String b)
    {
        return Internal.String_Equals(a, b);
    }

    public static Boolean operator !=(String a, String b)
    {
        return !Internal.String_Equals(a, b);
    }

    public static Boolean operator <(String a, String b) => Internal.String_Compare(a, b) < 0;

    public static Boolean operator <=(String a, String b)
    {
        return Internal.String_Compare(a, b) <= 0;
    }

    public static Boolean operator >(String a, String b) => Internal.String_Compare(a, b) > 0;

    public static Boolean operator >=(String a, String b)
    {
        return Internal.String_Compare(a, b) >= 0;
    }

};
