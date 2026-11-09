/*
 *  System.dll
 *
 *  Reflection: the base of the members of a type.
 */

namespace Reflection;

////////// ////////// ////////// ////////// //////////

/// <summary>
/// The base of the members of a type: fields, methods, properties and events. It has the type that
/// declares the member, its type, its access and its modifiers.
/// </summary>
[Internal]
public class Member : ReflectionInfo
{
    // The flags of the member: the access (access_value_t) in the 4 lowest bits, then the bits of
    // the modifiers, and the other bits of the kinds of members. (See rt_reflect_member_t.)
    protected Int64 Flags => Internal.Reflection_GetInfoFlags(this);

    /// <summary>
    /// Gets the type that declares the member. For a member that a generic type has, it is the
    /// generic type with its arguments (<c>List&lt;Int32&gt;</c>).
    /// </summary>
    public Type DeclaringType => Internal.Reflection_GetInfoDeclaringType(this);

    /// <summary>
    /// Gets the type of the member: the type of a field, the type of a property, the type of the
    /// event, the return type of a method.
    /// </summary>
    /// <remarks>
    /// It is null when the type is not known: the types that are made of the generic parameters of
    /// a generic method (<c>T Get&lt;T&gt;()</c>), of a generic type that has no arguments, and the
    /// types of arrays and others that a native program does not keep.
    /// </remarks>
    public Type? MemberType => Internal.Reflection_GetInfoType(this);

    /// <summary>Gets whether the member is public.</summary>
    public Boolean IsPublic => (Flags & 15) == 1;

    /// <summary>Gets whether the member is protected (and not internal).</summary>
    public Boolean IsProtected => (Flags & 15) == 2;

    /// <summary>Gets whether the member is private.</summary>
    public Boolean IsPrivate => (Flags & 15) == 3;

    /// <summary>Gets whether the member is internal (and not protected).</summary>
    public Boolean IsInternal => (Flags & 15) == 4;

    /// <summary>Gets whether the member is static.</summary>
    public Boolean IsStatic => (Flags & 16) != 0;

    /// <summary>Gets whether the member is sealed.</summary>
    public Boolean IsSealed => (Flags & 32) != 0;

    /// <summary>Gets whether the member overrides a member of the base type.</summary>
    public Boolean IsOverride => (Flags & 64) != 0;

    /// <summary>Gets whether the member is virtual.</summary>
    public Boolean IsVirtual => (Flags & 128) != 0;

    /// <summary>Gets whether the member is abstract.</summary>
    public Boolean IsAbstract => (Flags & 256) != 0;
}
