/*
 *  System.dll
 *
 *  StringHelper: what the compiler calls for the interpolated strings ($"..{x}..") and for the chains of + of strings (a + b + c).
 */

////////// ////////// ////////// ////////// //////////

// The compiler makes the text of each operand of a chain of + of strings with Text (the same text as the operator + of String has
// for it), and joins the texts with one String.Concat. A hole of an interpolated string that has a format or an alignment is a call of
// Part. See docs/string-format.md.
public static class StringHelper
{
    // The text of a value, as the operators + have it (the compiler joins the texts of a chain of + with one Concat).
    public static String Text(String? value) => value == null ? "" : value;

    public static String Text(Char value) => value.ToString();

    public static String Text(Boolean value) => value.ToString();

    public static String Text(Int8 value) => value.ToString();

    public static String Text(UInt8 value) => value.ToString();

    public static String Text(Int16 value) => value.ToString();

    public static String Text(UInt16 value) => value.ToString();

    public static String Text(Int32 value) => value.ToString();

    public static String Text(UInt32 value) => value.ToString();

    public static String Text(Int64 value) => value.ToString();

    public static String Text(UInt64 value) => value.ToString();

    public static String Text(Single value) => value.ToString();

    public static String Text(Double value) => value.ToString();

    public static String Text(Object? value) => Object.ToString(value);

    // A hole that has a format or an alignment: {value,alignment:format} (the format is empty when there is none). The numbers use their ToString(format); an object that is
    // IFormattable too; for the others the format is ignored (as in String.Format). A positive alignment is the width of the text at
    // its right, a negative one at its left.
    public static String Part(String? value, String format, Int32 alignment) => __Align(Text(value), alignment);

    public static String Part(Char value, String format, Int32 alignment) => __Align(value.ToString(), alignment);

    public static String Part(Boolean value, String format, Int32 alignment) => __Align(value.ToString(), alignment);

    public static String Part(Int8 value, String format, Int32 alignment)
        => __Align(format.Length == 0 ? value.ToString() : value.ToString(format), alignment);

    public static String Part(UInt8 value, String format, Int32 alignment)
        => __Align(format.Length == 0 ? value.ToString() : value.ToString(format), alignment);

    public static String Part(Int16 value, String format, Int32 alignment)
        => __Align(format.Length == 0 ? value.ToString() : value.ToString(format), alignment);

    public static String Part(UInt16 value, String format, Int32 alignment)
        => __Align(format.Length == 0 ? value.ToString() : value.ToString(format), alignment);

    public static String Part(Int32 value, String format, Int32 alignment)
        => __Align(format.Length == 0 ? value.ToString() : value.ToString(format), alignment);

    public static String Part(UInt32 value, String format, Int32 alignment)
        => __Align(format.Length == 0 ? value.ToString() : value.ToString(format), alignment);

    public static String Part(Int64 value, String format, Int32 alignment)
        => __Align(format.Length == 0 ? value.ToString() : value.ToString(format), alignment);

    public static String Part(UInt64 value, String format, Int32 alignment)
        => __Align(format.Length == 0 ? value.ToString() : value.ToString(format), alignment);

    public static String Part(Single value, String format, Int32 alignment)
        => __Align(format.Length == 0 ? value.ToString() : value.ToString(format), alignment);

    public static String Part(Double value, String format, Int32 alignment)
        => __Align(format.Length == 0 ? value.ToString() : value.ToString(format), alignment);

    public static String Part<T>(T value, String format, Int32 alignment) => __Align(Object.Format((Object)value, format), alignment);

    private static String __Align(String text, Int32 alignment)
    {
        if (alignment > 0 && text.Length < alignment)
            return text.PadLeft(alignment);

        if (alignment < 0 && text.Length < -alignment)
            return text.PadRight(-alignment);

        return text;
    }
}
