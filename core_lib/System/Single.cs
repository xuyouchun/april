
/*
 *  System.dll
 *
 *  Single: the members of the type (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct Single
{
    public Int32 CompareTo(Single other) => Primitives.Compare(this, other);

    public Boolean Equals(Single other) => Primitives.Same(this, other);

    public Boolean Equals(Object? obj) => obj is Single && Primitives.Same(this, (Single)obj);

    public Int32 GetHashCode() => Primitives.Hash(this);

    public String ToString() => Internal.Single_ToString(this);

    public String ToString(String? format)
        => NumberFormat.FormatFloating((Double)this, Internal.Single_ToString(this), format, Globalization.NumberFormatInfo.CurrentInfo);

    public String ToString(String? format, IFormatProvider? provider)
        => NumberFormat.FormatFloating((Double)this, Internal.Single_ToString(this), format, Globalization.NumberFormatInfo.GetInstance(provider));

    // The number of a text: spaces, a sign and digits (see NumberParse). FormatException, OverflowException.
    public static Single Parse(String text) => Parse(text, null);

    public static Single Parse(String text, IFormatProvider? provider)
    {
        String normalized;
        Int32 kind = NumberParse.Floating(text, Globalization.NumberFormatInfo.GetInstance(provider), out normalized);
        if (kind == 0)
            NumberParse.Throw(1, text, "Single");

        return __Make(kind, normalized);
    }

    public static Boolean TryParse(String? text, out Single result) => TryParse(text, null, out result);

    public static Boolean TryParse(String? text, IFormatProvider? provider, out Single result)
    {
        String normalized;
        Int32 kind = NumberParse.Floating(text, Globalization.NumberFormatInfo.GetInstance(provider), out normalized);
        if (kind == 0)
        {
            result = 0;
            return false;
        }

        result = __Make(kind, normalized);
        return true;
    }

    private static Single __Make(Int32 kind, String normalized)
    {
        if (kind == 2)
            return (Single)NumberParse.NaNValue();

        if (kind == 3)
            return (Single)NumberParse.InfinityValue();

        if (kind == 4)
            return (Single)(-NumberParse.InfinityValue());

        return Internal.Single_Parse(normalized);
    }
}
