/*
 *  System.dll
 *
 *  The interfaces of the types: text, hash code, equality, order,
 *  and releasing resources.
 */

using Reflection;

////////// ////////// ////////// ////////// //////////

// A type that releases its resources: the using statement calls Dispose() on exit, and
// foreach calls it on its enumerator.
public interface IDisposable
{
    Void Dispose();
}

////////// ////////// ////////// ////////// //////////

// A type whose values are ordered.
public interface IComparable<in T>
{
    // Returns a negative number if this is before other, zero if they are equal, a positive
    // number otherwise.
    Int32 CompareTo(T other);
}

////////// ////////// ////////// ////////// //////////

// A type whose values are compared for equality.
public interface IEquatable<in T>
{
    Boolean Equals(T other);
}

////////// ////////// ////////// ////////// //////////

// A type whose objects are compared for equality as objects: Equals(Object) and GetHashCode() go
// together, a type that has one has the other. A type that does not have it is equal by identity
// (the same object). IEquatable<T> is the equality with a value of a type, IEqualityComparer<T>
// is the equality that is made by another object; Object.Equals and Object.HashOf use this one,
// or the identity.
public interface IEquality
{
    Boolean Equals(Object? obj);

    Int32 GetHashCode();
}

////////// ////////// ////////// ////////// //////////

// An object that gives the object that formats a type: NumberFormatInfo is one (it is the format provider of the numbers).
public interface IFormatProvider
{
    // The object of the type that formats (a NumberFormatInfo for the numbers), or null.
    Object? GetFormat(Type? formatType);
}

////////// ////////// ////////// ////////// //////////

// A type whose values have a text by a format: the numbers (Int32.ToString("X"), Double.ToString("F2")), and what String.Format
// calls for {0:format}. The provider gives the symbols (NumberFormatInfo); without one they are those of CurrentInfo.
public interface IFormattable
{
    String ToString(String? format);

    String ToString(String? format, IFormatProvider? provider);
}

////////// ////////// ////////// ////////// //////////

// A type that has a text of its own: Object.ToString(obj) returns it, the full
// name of the type is the text of the types that do not have it.
public interface IStringable
{
    String ToString();
}
