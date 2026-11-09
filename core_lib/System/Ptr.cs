/*
 *  System.dll
 *
 *  Ptr, Ptr<T>: the members of the types (the declarations are in +types.cs).
 *
 *  Ptr is a pointer to nothing in particular (void*), Ptr<T> a pointer to a value of the unmanaged type T (T*).
 *  Both are an address of 8 bytes, a value like a number.
 *
 *  The members that read and write the memory (Value, the indexer) and the operators (+, -, ++, --, the compares) are
 *  made by the compiler: each of them is one or a few instructions, no call. Their bodies are never executed.
 */

////////// ////////// ////////// ////////// //////////

public partial struct Ptr
{
    // The pointer that points to nothing.
    public static Ptr Null => Internal.Ptr_FromInt64(0);

    // The pointer to an address.
    public static Ptr FromInt64(Int64 address) => Internal.Ptr_FromInt64(address);

    // The address.
    public Int64 ToInt64() => Internal.Ptr_ToInt64(this);

    public Boolean IsNull => Internal.Ptr_ToInt64(this) == 0;

    // The same address, seen as a pointer to a value of the type T.
    public Ptr<T> As<T>() where T : unmanaged => (Ptr<T>)this;

    // The address of the first value of an array: the pointer is valid while the array is alive, and while the method that
    // made it runs (the compiler keeps the array in a variable of the method). An array that has no value gives an error.
    public static Ptr<T> AddressOf<T>(T[] array) where T : unmanaged { throw new InvalidOperationException("made by the compiler"); }

    // The address of a value of an array (the index is checked).
    public static Ptr<T> AddressOf<T>(T[] array, Int32 index) where T : unmanaged { throw new InvalidOperationException("made by the compiler"); }

    // The address of a variable, a field or an element: Ptr.AddressOf(ref x).
    public static Ptr<T> AddressOf<T>(ref T value) where T : unmanaged { throw new InvalidOperationException("made by the compiler"); }

    // The address of the first character of a string, of the character of an index (0 to Length): the string must be alive
    // (kept by a variable) while the pointer is used, its characters are not written.
    public static Ptr<Char> AddressOf(String s) => (Ptr<Char>)Internal.String_AddressOf(s, 0);

    public static Ptr<Char> AddressOf(String s, Int32 index)
    {
        if (index < 0 || index > s.Length)
            throw new OutOfRangeException();

        return (Ptr<Char>)Internal.String_AddressOf(s, index);
    }

    // The address moves by bytes.
    public static Ptr operator +(Ptr p, Int64 bytes) { throw new InvalidOperationException("made by the compiler"); }

    public static Ptr operator -(Ptr p, Int64 bytes) { throw new InvalidOperationException("made by the compiler"); }

    // The bytes between two addresses.
    public static Int64 operator -(Ptr p, Ptr q) { throw new InvalidOperationException("made by the compiler"); }

    public static Ptr operator ++(Ptr p) { throw new InvalidOperationException("made by the compiler"); }

    public static Ptr operator --(Ptr p) { throw new InvalidOperationException("made by the compiler"); }

    public static Boolean operator ==(Ptr p, Ptr q) { throw new InvalidOperationException("made by the compiler"); }

    public static Boolean operator !=(Ptr p, Ptr q) { throw new InvalidOperationException("made by the compiler"); }

    public static Boolean operator <(Ptr p, Ptr q) { throw new InvalidOperationException("made by the compiler"); }

    public static Boolean operator <=(Ptr p, Ptr q) { throw new InvalidOperationException("made by the compiler"); }

    public static Boolean operator >(Ptr p, Ptr q) { throw new InvalidOperationException("made by the compiler"); }

    public static Boolean operator >=(Ptr p, Ptr q) { throw new InvalidOperationException("made by the compiler"); }

    public Boolean Equals(Ptr other) => Internal.Ptr_ToInt64(this) == Internal.Ptr_ToInt64(other);

    public Boolean Equals(Object? obj) => obj is Ptr && Internal.Ptr_ToInt64(this) == Internal.Ptr_ToInt64((Ptr)obj);

    public Int32 GetHashCode() => Internal.Ptr_ToInt64(this).GetHashCode();

    public String ToString() => "0x" + Internal.Ptr_ToInt64(this).ToString("X16");
}

////////// ////////// ////////// ////////// //////////

public partial struct Ptr<T>
{
    // The pointer that points to nothing.
    public static Ptr<T> Null => (Ptr<T>)Internal.Ptr_FromInt64(0);

    // The pointer to an address.
    public static Ptr<T> FromInt64(Int64 address) => (Ptr<T>)Internal.Ptr_FromInt64(address);

    // The address.
    public Int64 ToInt64() => Internal.Ptr_ToInt64(this);

    public Boolean IsNull => Internal.Ptr_ToInt64(this) == 0;

    // The same address, as a pointer to nothing in particular (void*).
    public Ptr ToPtr() => this;

    // The same address, seen as a pointer to a value of the type U.
    public Ptr<U> As<U>() where U : unmanaged => (Ptr<U>)this;

    // The value that the pointer points to: *p. There is no check of the address (null too).
    public T Value
    {
        get { throw new InvalidOperationException("made by the compiler"); }
        set { throw new InvalidOperationException("made by the compiler"); }
    }

    // The value that is i values after the one that the pointer points to: p[i], *(p + i).
    public T this[Int64 index]
    {
        get { throw new InvalidOperationException("made by the compiler"); }
        set { throw new InvalidOperationException("made by the compiler"); }
    }

    // The address moves by values (of the size of T).
    public static Ptr<T> operator +(Ptr<T> p, Int64 count) { throw new InvalidOperationException("made by the compiler"); }

    public static Ptr<T> operator -(Ptr<T> p, Int64 count) { throw new InvalidOperationException("made by the compiler"); }

    // The number of values between two addresses.
    public static Int64 operator -(Ptr<T> p, Ptr<T> q) { throw new InvalidOperationException("made by the compiler"); }

    public static Ptr<T> operator ++(Ptr<T> p) { throw new InvalidOperationException("made by the compiler"); }

    public static Ptr<T> operator --(Ptr<T> p) { throw new InvalidOperationException("made by the compiler"); }

    public static Boolean operator ==(Ptr<T> p, Ptr<T> q) { throw new InvalidOperationException("made by the compiler"); }

    public static Boolean operator !=(Ptr<T> p, Ptr<T> q) { throw new InvalidOperationException("made by the compiler"); }

    public static Boolean operator <(Ptr<T> p, Ptr<T> q) { throw new InvalidOperationException("made by the compiler"); }

    public static Boolean operator <=(Ptr<T> p, Ptr<T> q) { throw new InvalidOperationException("made by the compiler"); }

    public static Boolean operator >(Ptr<T> p, Ptr<T> q) { throw new InvalidOperationException("made by the compiler"); }

    public static Boolean operator >=(Ptr<T> p, Ptr<T> q) { throw new InvalidOperationException("made by the compiler"); }

    public Boolean Equals(Ptr<T> other) => Internal.Ptr_ToInt64(this) == Internal.Ptr_ToInt64(other);

    public Boolean Equals(Object? obj) => obj is Ptr<T> && Internal.Ptr_ToInt64(this) == Internal.Ptr_ToInt64((Ptr<T>)obj);

    public Int32 GetHashCode() => Internal.Ptr_ToInt64(this).GetHashCode();

    public String ToString() => "0x" + Internal.Ptr_ToInt64(this).ToString("X16");
}
