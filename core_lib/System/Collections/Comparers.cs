/*
 *  System.dll
 *
 *  Comparers of integers and strings.
 */

using Reflection;

namespace Collections;

////////// ////////// ////////// ////////// //////////

// Compares integers.
public class Int32Comparer : IComparer<Int32>, IEqualityComparer<Int32>
{
    public Int32 Compare(Int32 x, Int32 y)
    {
        if (x < y)
            return -1;

        if (x > y)
            return 1;

        return 0;
    }

    public Boolean Equals(Int32 x, Int32 y) => x == y;

    public Int32 GetHashCode(Int32 obj) => obj & 2147483647;
}

// Compares long integers.
public class Int64Comparer : IComparer<Int64>, IEqualityComparer<Int64>
{
    public Int32 Compare(Int64 x, Int64 y)
    {
        if (x < y)
            return -1;

        if (x > y)
            return 1;

        return 0;
    }

    public Boolean Equals(Int64 x, Int64 y) => x == y;

    public Int32 GetHashCode(Int64 obj) => (Int32)(obj & 2147483647);
}

// Compares strings, by the characters.
public class StringComparer : IComparer<String>, IEqualityComparer<String>
{
    public Int32 Compare(String x, String y) => Internal.String_Compare(x, y);

    public Boolean Equals(String x, String y) => Internal.String_Equals(x, y);

    public Int32 GetHashCode(String obj) => Internal.String_GetHashCode(obj);
}

////////// ////////// ////////// ////////// //////////

// Compares the values of Int8.
public class Int8Comparer : IComparer<Int8>, IEqualityComparer<Int8>
{
    public Int32 Compare(Int8 x, Int8 y) => Primitives.Compare(x, y);

    public Boolean Equals(Int8 x, Int8 y) => Primitives.Same(x, y);

    public Int32 GetHashCode(Int8 obj) => Primitives.Hash(obj);
}

// Compares the values of UInt8.
public class UInt8Comparer : IComparer<UInt8>, IEqualityComparer<UInt8>
{
    public Int32 Compare(UInt8 x, UInt8 y) => Primitives.Compare(x, y);

    public Boolean Equals(UInt8 x, UInt8 y) => Primitives.Same(x, y);

    public Int32 GetHashCode(UInt8 obj) => Primitives.Hash(obj);
}

// Compares the values of Int16.
public class Int16Comparer : IComparer<Int16>, IEqualityComparer<Int16>
{
    public Int32 Compare(Int16 x, Int16 y) => Primitives.Compare(x, y);

    public Boolean Equals(Int16 x, Int16 y) => Primitives.Same(x, y);

    public Int32 GetHashCode(Int16 obj) => Primitives.Hash(obj);
}

// Compares the values of UInt16.
public class UInt16Comparer : IComparer<UInt16>, IEqualityComparer<UInt16>
{
    public Int32 Compare(UInt16 x, UInt16 y) => Primitives.Compare(x, y);

    public Boolean Equals(UInt16 x, UInt16 y) => Primitives.Same(x, y);

    public Int32 GetHashCode(UInt16 obj) => Primitives.Hash(obj);
}

// Compares the values of UInt32.
public class UInt32Comparer : IComparer<UInt32>, IEqualityComparer<UInt32>
{
    public Int32 Compare(UInt32 x, UInt32 y) => Primitives.Compare(x, y);

    public Boolean Equals(UInt32 x, UInt32 y) => Primitives.Same(x, y);

    public Int32 GetHashCode(UInt32 obj) => Primitives.Hash(obj);
}

// Compares the values of UInt64.
public class UInt64Comparer : IComparer<UInt64>, IEqualityComparer<UInt64>
{
    public Int32 Compare(UInt64 x, UInt64 y) => Primitives.Compare(x, y);

    public Boolean Equals(UInt64 x, UInt64 y) => Primitives.Same(x, y);

    public Int32 GetHashCode(UInt64 obj) => Primitives.Hash(obj);
}

// Compares the values of Char.
public class CharComparer : IComparer<Char>, IEqualityComparer<Char>
{
    public Int32 Compare(Char x, Char y) => Primitives.Compare(x, y);

    public Boolean Equals(Char x, Char y) => Primitives.Same(x, y);

    public Int32 GetHashCode(Char obj) => Primitives.Hash(obj);
}

// Compares the values of Single.
public class SingleComparer : IComparer<Single>, IEqualityComparer<Single>
{
    public Int32 Compare(Single x, Single y) => Primitives.Compare(x, y);

    public Boolean Equals(Single x, Single y) => Primitives.Same(x, y);

    public Int32 GetHashCode(Single obj) => Primitives.Hash(obj);
}

// Compares the values of Double.
public class DoubleComparer : IComparer<Double>, IEqualityComparer<Double>
{
    public Int32 Compare(Double x, Double y) => Primitives.Compare(x, y);

    public Boolean Equals(Double x, Double y) => Primitives.Same(x, y);

    public Int32 GetHashCode(Double obj) => Primitives.Hash(obj);
}

// Compares the values of Boolean.
public class BooleanComparer : IComparer<Boolean>, IEqualityComparer<Boolean>
{
    public Int32 Compare(Boolean x, Boolean y) => Primitives.Compare(x, y);

    public Boolean Equals(Boolean x, Boolean y) => Primitives.Same(x, y);

    public Int32 GetHashCode(Boolean obj) => Primitives.Hash(obj);
}

////////// ////////// ////////// ////////// //////////

// Orders the values of a type that implements IComparable<T> (a null is before the others).
public class ComparableComparer<T> : IComparer<T>
{
    public Int32 Compare(T x, T y)
    {
        Object a = x;
        Object b = y;

        if (a == null)
            return b == null? 0 : -1;

        if (b == null)
            return 1;

        IComparable<T> comparable = a as IComparable<T>;
        if (comparable == null)
            throw new InvalidOperationException("The type of the values is not comparable.");

        return comparable.CompareTo(y);
    }
}

// Compares the values of a type for equality: IEquatable<T>, or IEquality (the same object, when the
// type does not have it).
public class ObjectComparer<T> : IEqualityComparer<T>
{
    public Boolean Equals(T x, T y)
    {
        Object a = x;
        Object b = y;

        if (a == null)
            return b == null;

        if (b == null)
            return false;

        IEquatable<T> equatable = a as IEquatable<T>;
        if (equatable != null)
            return equatable.Equals(y);

        return Object.Equals(a, b);
    }

    public Int32 GetHashCode(T obj)
    {
        Object a = obj;
        return Object.HashOf(a);
    }
}

////////// ////////// ////////// ////////// //////////

// The default order of the values of a type: the one of the primitive types, or IComparable<T>.
public static class Comparer<T>
{
    public static IComparer<T> Default => Comparers.OrderOf<T>();
}

// The default equality of the values of a type: the one of the primitive types, or IEquatable<T>,
// or IEquality of the objects.
public static class EqualityComparer<T>
{
    public static IEqualityComparer<T> Default => Comparers.EqualityOf<T>();
}

// Finds the comparer of a type by its type (the static members of a generic type are not kept for
// each type, a comparer is made each time).
internal static class Comparers
{
    public static IComparer<T> OrderOf<T>()
    {
        Type type = typeof(T);

        if (type.Equals(typeof(Int8)))
            return (IComparer<T>)(Object)new Int8Comparer();

        if (type.Equals(typeof(UInt8)))
            return (IComparer<T>)(Object)new UInt8Comparer();

        if (type.Equals(typeof(Int16)))
            return (IComparer<T>)(Object)new Int16Comparer();

        if (type.Equals(typeof(UInt16)))
            return (IComparer<T>)(Object)new UInt16Comparer();

        if (type.Equals(typeof(Int32)))
            return (IComparer<T>)(Object)new Int32Comparer();

        if (type.Equals(typeof(UInt32)))
            return (IComparer<T>)(Object)new UInt32Comparer();

        if (type.Equals(typeof(Int64)))
            return (IComparer<T>)(Object)new Int64Comparer();

        if (type.Equals(typeof(UInt64)))
            return (IComparer<T>)(Object)new UInt64Comparer();

        if (type.Equals(typeof(Char)))
            return (IComparer<T>)(Object)new CharComparer();

        if (type.Equals(typeof(Single)))
            return (IComparer<T>)(Object)new SingleComparer();

        if (type.Equals(typeof(Double)))
            return (IComparer<T>)(Object)new DoubleComparer();

        if (type.Equals(typeof(Boolean)))
            return (IComparer<T>)(Object)new BooleanComparer();

        if (type.Equals(typeof(String)))
            return (IComparer<T>)(Object)new StringComparer();

        return new ComparableComparer<T>();
    }

    public static IEqualityComparer<T> EqualityOf<T>()
    {
        Type type = typeof(T);

        if (type.Equals(typeof(Int8)))
            return (IEqualityComparer<T>)(Object)new Int8Comparer();

        if (type.Equals(typeof(UInt8)))
            return (IEqualityComparer<T>)(Object)new UInt8Comparer();

        if (type.Equals(typeof(Int16)))
            return (IEqualityComparer<T>)(Object)new Int16Comparer();

        if (type.Equals(typeof(UInt16)))
            return (IEqualityComparer<T>)(Object)new UInt16Comparer();

        if (type.Equals(typeof(Int32)))
            return (IEqualityComparer<T>)(Object)new Int32Comparer();

        if (type.Equals(typeof(UInt32)))
            return (IEqualityComparer<T>)(Object)new UInt32Comparer();

        if (type.Equals(typeof(Int64)))
            return (IEqualityComparer<T>)(Object)new Int64Comparer();

        if (type.Equals(typeof(UInt64)))
            return (IEqualityComparer<T>)(Object)new UInt64Comparer();

        if (type.Equals(typeof(Char)))
            return (IEqualityComparer<T>)(Object)new CharComparer();

        if (type.Equals(typeof(Single)))
            return (IEqualityComparer<T>)(Object)new SingleComparer();

        if (type.Equals(typeof(Double)))
            return (IEqualityComparer<T>)(Object)new DoubleComparer();

        if (type.Equals(typeof(Boolean)))
            return (IEqualityComparer<T>)(Object)new BooleanComparer();

        if (type.Equals(typeof(String)))
            return (IEqualityComparer<T>)(Object)new StringComparer();

        return new ObjectComparer<T>();
    }
}
