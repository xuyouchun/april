
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
}
