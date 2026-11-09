/*
 *  System.dll
 *
 *  Reflection: a generic parameter of a type or of a method.
 */

namespace Reflection;

////////// ////////// ////////// ////////// //////////

/// <summary>
/// A generic parameter of a type or of a method: the <c>T</c> of <c>List&lt;T&gt;</c>.
/// </summary>
[Internal]
public sealed class GenericParameter : ReflectionInfo
{
    // The bits of the flags: the kind of the parameter (generic_param_type_t: 1 in, 2 out,
    // 3 a pack of arguments) in the bits 16 to 23, and its position in the bits 24 to 39.
    private Int64 __Flags => Internal.Reflection_GetInfoFlags(this);

    /// <summary>
    /// Gets the position of the parameter in the list of the generic parameters, the first is 0.
    /// </summary>
    public Int32 Position => (Int32)((__Flags >> 24) & 65535);

    /// <summary>
    /// Gets whether the parameter is contravariant (<c>in T</c>): it is only used to read values.
    /// </summary>
    public Boolean IsContravariant => ((__Flags >> 16) & 255) == 1;

    /// <summary>
    /// Gets whether the parameter is covariant (<c>out T</c>): it is only used to write values.
    /// </summary>
    public Boolean IsCovariant => ((__Flags >> 16) & 255) == 2;

    /// <summary>
    /// Gets whether the parameter stands for many types (<c>TArgs ...</c>).
    /// </summary>
    public Boolean IsParams => ((__Flags >> 16) & 255) == 3;
}
