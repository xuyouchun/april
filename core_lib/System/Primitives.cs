/*
 *  System.dll
 *
 *  The order, the equality and the hash code of the primitive types: they are the members of
 *  IComparable<T>, IEquatable<T> and GetHashCode of the types.
 */

////////// ////////// ////////// ////////// //////////

internal static class Primitives
{

    public static Int32 Compare(Int8 x, Int8 y) => (Int32)x < (Int32)y? -1 : ((Int32)x > (Int32)y? 1 : 0);

    public static Boolean Same(Int8 x, Int8 y) => (Int32)x == (Int32)y;

    public static Int32 Hash(Int8 x) => (Int32)x & 2147483647;


    public static Int32 Compare(UInt8 x, UInt8 y) => (Int32)x < (Int32)y? -1 : ((Int32)x > (Int32)y? 1 : 0);

    public static Boolean Same(UInt8 x, UInt8 y) => (Int32)x == (Int32)y;

    public static Int32 Hash(UInt8 x) => (Int32)x & 2147483647;


    public static Int32 Compare(Int16 x, Int16 y) => (Int32)x < (Int32)y? -1 : ((Int32)x > (Int32)y? 1 : 0);

    public static Boolean Same(Int16 x, Int16 y) => (Int32)x == (Int32)y;

    public static Int32 Hash(Int16 x) => (Int32)x & 2147483647;


    public static Int32 Compare(UInt16 x, UInt16 y) => (Int32)x < (Int32)y? -1 : ((Int32)x > (Int32)y? 1 : 0);

    public static Boolean Same(UInt16 x, UInt16 y) => (Int32)x == (Int32)y;

    public static Int32 Hash(UInt16 x) => (Int32)x & 2147483647;


    public static Int32 Compare(Int32 x, Int32 y) => x < y? -1 : (x > y? 1 : 0);

    public static Boolean Same(Int32 x, Int32 y) => x == y;

    public static Int32 Hash(Int32 x) => (Int32)x & 2147483647;


    public static Int32 Compare(UInt32 x, UInt32 y) => x < y? -1 : (x > y? 1 : 0);

    public static Boolean Same(UInt32 x, UInt32 y) => x == y;

    public static Int32 Hash(UInt32 x) => (Int32)x & 2147483647;


    public static Int32 Compare(Int64 x, Int64 y) => x < y? -1 : (x > y? 1 : 0);

    public static Boolean Same(Int64 x, Int64 y) => x == y;

    public static Int32 Hash(Int64 x) => (Int32)(x & 2147483647);


    public static Int32 Compare(UInt64 x, UInt64 y) => x < y? -1 : (x > y? 1 : 0);

    public static Boolean Same(UInt64 x, UInt64 y) => x == y;

    public static Int32 Hash(UInt64 x) => (Int32)(x & 2147483647);


    public static Int32 Compare(Char x, Char y) => (Int32)x < (Int32)y? -1 : ((Int32)x > (Int32)y? 1 : 0);

    public static Boolean Same(Char x, Char y) => (Int32)x == (Int32)y;

    public static Int32 Hash(Char x) => (Int32)x & 2147483647;


    public static Int32 Compare(Single x, Single y) => x < y? -1 : (x > y? 1 : 0);

    public static Boolean Same(Single x, Single y) => x == y;

    public static Int32 Hash(Single x) => (x < 2147483647 && x > -2147483647)? ((Int32)x & 2147483647) : 0;


    public static Int32 Compare(Double x, Double y) => x < y? -1 : (x > y? 1 : 0);

    public static Boolean Same(Double x, Double y) => x == y;

    public static Int32 Hash(Double x) => (x < 2147483647 && x > -2147483647)? ((Int32)x & 2147483647) : 0;


    public static Int32 Compare(Boolean x, Boolean y) => x == y? 0 : (x? 1 : -1);

    public static Boolean Same(Boolean x, Boolean y) => x == y;

    public static Int32 Hash(Boolean x) => x? 1 : 0;
}
