
/*
 *  System.dll
 *
 *  Double: the members of the type (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct Double
{
    public Int32 CompareTo(Double other) => Primitives.Compare(this, other);

    public Boolean Equals(Double other) => Primitives.Same(this, other);

    public Boolean Equals(Object? obj) => obj is Double && Primitives.Same(this, (Double)obj);

    public Int32 GetHashCode() => Primitives.Hash(this);

    public String ToString() => Internal.Double_ToString(this);

    public String ToString(String? format)
        => NumberFormat.FormatFloating(this, Internal.Double_ToString(this), format, Globalization.NumberFormatInfo.CurrentInfo);

    public String ToString(String? format, IFormatProvider? provider)
        => NumberFormat.FormatFloating(this, Internal.Double_ToString(this), format, Globalization.NumberFormatInfo.GetInstance(provider));

    // The number of a text: spaces, a sign and digits (see NumberParse). FormatException, OverflowException.
    public static Double Parse(String text) => Parse(text, null);

    public static Double Parse(String text, IFormatProvider? provider)
    {
        String normalized;
        Int32 kind = NumberParse.Floating(text, Globalization.NumberFormatInfo.GetInstance(provider), out normalized);
        if (kind == 0)
            NumberParse.Throw(1, text, "Double");

        return __Make(kind, normalized);
    }

    public static Boolean TryParse(String? text, out Double result) => TryParse(text, null, out result);

    public static Boolean TryParse(String? text, IFormatProvider? provider, out Double result)
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

    private static Double __Make(Int32 kind, String normalized)
    {
        if (kind == 2)
            return (Double)NumberParse.NaNValue();

        if (kind == 3)
            return (Double)NumberParse.InfinityValue();

        if (kind == 4)
            return (Double)(-NumberParse.InfinityValue());

        return Internal.Double_Parse(normalized);
    }
}
