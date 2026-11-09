
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
}
