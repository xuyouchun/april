/*
 *  System.dll
 *
 *  The interfaces of the types: text, hash code, equality, order,
 *  and releasing resources.
 */

////////// ////////// ////////// ////////// //////////

// Converts to string.
public interface IString
{
    String ToString();
}

// Get hash code.
public interface IHashCode
{
    Int32 GetHashCode();
}

// Check if two objects are equal.
public interface IEquals
{
    Boolean Equals(Object obj);
}

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
