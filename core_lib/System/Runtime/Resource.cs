/*
 *  System.dll
 *
 *  Resource: the files that are embedded in the assemblies.
 */

namespace Runtime;

////////// ////////// ////////// ////////// //////////

// The resources of the assemblies: files that are in the <resources> of a project, embedded in the assembly that it
// makes. A resource has a name: the one of the project and the path from its directory with . in place of the
// separators (System.Text.gb18030.bin), or the one that the project gives. The assemblies that are loaded are searched
// by the name, the first that has it gives the bytes.
public static class Resource
{
    // Whether an assembly has the resource.
    public static Boolean Exists(String name) => Internal.Resource_GetSize(name) >= 0;

    // The number of bytes of the resource; an ArgumentException when there is none of the name.
    public static Int32 GetSize(String name)
    {
        Int32 size = Internal.Resource_GetSize(name);
        if (size < 0)
            throw new ArgumentException("the resource '" + name + "' is not found");

        return size;
    }

    // A new array with the bytes of the resource; an ArgumentException when there is none of the name.
    public static UInt8[] GetBytes(String name)
    {
        UInt8[] bytes = new UInt8[GetSize(name)];
        Internal.Resource_Read(name, bytes);

        return bytes;
    }

    // The address of the bytes of the resource (GetSize tells how many): they are in the memory of the program, they are
    // not copied and not written. A null pointer when there is no resource of the name.
    public static Ptr<UInt8> GetPtr(String name) => (Ptr<UInt8>)Internal.Resource_GetPtr(name);

    // The bytes of the resource as a read-only span.
    public static ReadOnlySpan<UInt8> GetSpan(String name) => new ReadOnlySpan<UInt8>(GetBytes(name));
}
