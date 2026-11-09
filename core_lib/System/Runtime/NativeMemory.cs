/*
 *  System.dll
 *
 *  NativeMemory: the memory of the program that is not collected, for the pointers (Ptr, Ptr<T>).
 */

namespace Runtime;

////////// ////////// ////////// ////////// //////////

// Memory that is not of the heap that is collected: it is given back by Free, it does not move. A pointer to it can be
// kept anywhere.
public static class NativeMemory
{
    // The memory of the bytes; not cleared. Null when there is not enough.
    public static Ptr Alloc(Int64 bytes) => Internal.Memory_Alloc(bytes);

    // Gives the memory back (a null pointer is nothing).
    public static Void Free(Ptr ptr) => Internal.Memory_Free(ptr);

    public static Void Free<T>(Ptr<T> ptr) where T : unmanaged => Internal.Memory_Free(ptr);

    // The size of a value of the type T in the memory (made by the compiler).
    public static Int64 SizeOf<T>() where T : unmanaged { throw new InvalidOperationException("made by the compiler"); }

    // Memory for count values of the type T that lives while the method that calls it runs (it is not a call of the library:
    // the compiler makes it): the pointer must not be returned or kept in a field. The content is not defined.
    public static Ptr<T> StackAlloc<T>(Int64 count) where T : unmanaged { throw new InvalidOperationException("made by the compiler"); }

    // The memory of count values of the type T; not cleared. Null when there is not enough.
    public static Ptr<T> Alloc<T>(Int64 count) where T : unmanaged => (Ptr<T>)Internal.Memory_Alloc(count * SizeOf<T>());

    // Copies count values of the type T (the ranges may overlap). Copy copies bytes, of the pointers of any type.
    public static Void CopyValues<T>(Ptr<T> destination, Ptr<T> source, Int64 count) where T : unmanaged
        => Internal.Memory_Copy(destination, source, count * SizeOf<T>());

    // Copies the bytes (the ranges may overlap).
    public static Void Copy(Ptr destination, Ptr source, Int64 bytes) => Internal.Memory_Copy(destination, source, bytes);

    // Sets the bytes to a value.
    public static Void Fill(Ptr destination, UInt8 value, Int64 bytes) => Internal.Memory_Fill(destination, value, bytes);
}
