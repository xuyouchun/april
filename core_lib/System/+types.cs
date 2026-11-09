
/*
 *  System.dll
 *
 */

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Void
{

};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct Int8 : IComparable<Int8>, IEquatable<Int8>, IEquality, IStringable, IFormattable, Numerics.IBinaryInteger<Int8>, IMinMaxValue<Int8>, Numerics.ISignedNumber<Int8>
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct UInt8 : IComparable<UInt8>, IEquatable<UInt8>, IEquality, IStringable, IFormattable, Numerics.IBinaryInteger<UInt8>, IMinMaxValue<UInt8>, Numerics.IUnsignedNumber<UInt8>
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct Int16 : IComparable<Int16>, IEquatable<Int16>, IEquality, IStringable, IFormattable, Numerics.IBinaryInteger<Int16>, IMinMaxValue<Int16>, Numerics.ISignedNumber<Int16>
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct UInt16 : IComparable<UInt16>, IEquatable<UInt16>, IEquality, IStringable, IFormattable, Numerics.IBinaryInteger<UInt16>, IMinMaxValue<UInt16>, Numerics.IUnsignedNumber<UInt16>
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct Int32 : IComparable<Int32>, IEquatable<Int32>, IEquality, IStringable, IFormattable, Numerics.IBinaryInteger<Int32>, IMinMaxValue<Int32>, Numerics.ISignedNumber<Int32>
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct UInt32 : IComparable<UInt32>, IEquatable<UInt32>, IEquality, IStringable, IFormattable, Numerics.IBinaryInteger<UInt32>, IMinMaxValue<UInt32>, Numerics.IUnsignedNumber<UInt32>
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct Int64 : IComparable<Int64>, IEquatable<Int64>, IEquality, IStringable, IFormattable, Numerics.IBinaryInteger<Int64>, IMinMaxValue<Int64>, Numerics.ISignedNumber<Int64>
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct UInt64 : IComparable<UInt64>, IEquatable<UInt64>, IEquality, IStringable, IFormattable, Numerics.IBinaryInteger<UInt64>, IMinMaxValue<UInt64>, Numerics.IUnsignedNumber<UInt64>
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct String : IComparable<String>, IEquatable<String>, IEquality, IStringable
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct Boolean : IComparable<Boolean>, IEquatable<Boolean>, IEquality, IStringable
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct Char : IComparable<Char>, IEquatable<Char>, IEquality, IStringable
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct Single : IComparable<Single>, IEquatable<Single>, IEquality, IStringable, IFormattable
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct Double : IComparable<Double>, IEquatable<Double>, IEquality, IStringable, IFormattable
{
};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct Ptr : IEquatable<Ptr>
{

};

////////// ////////// ////////// ////////// //////////

[Internal]
public partial struct Ptr<T> where T : unmanaged
{

};

////////// ////////// ////////// ////////// //////////


