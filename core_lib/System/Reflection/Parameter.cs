/*
 *  System.dll
 *
 *  Reflection: a parameter of a method.
 */

namespace Reflection;

////////// ////////// ////////// ////////// //////////

/// <summary>
/// A parameter of a method, or an index parameter of a property.
/// </summary>
[Internal]
public sealed class Parameter : ReflectionInfo
{
    // The bits of the flags: the kind of the parameter (param_type_t: 1 ref, 2 out, 3 params,
    // 4 a pack of generic arguments) in the bits 16 to 23, and its position in the bits 24 to 39.
    private Int64 __Flags => Internal.Reflection_GetInfoFlags(this);

    /// <summary>
    /// Gets the type of the parameter. It is null when the type is not known: a type that is made
    /// of the generic parameters of a generic method, and the types that a native program does not
    /// keep.
    /// </summary>
    public Type? ParameterType => Internal.Reflection_GetInfoType(this);

    /// <summary>
    /// Gets the position of the parameter in the list of the parameters, the first is 0.
    /// </summary>
    public Int32 Position => (Int32)((__Flags >> 24) & 65535);

    /// <summary>Gets whether the parameter is passed by reference (<c>ref</c>).</summary>
    public Boolean IsRef => ((__Flags >> 16) & 255) == 1;

    /// <summary>Gets whether the parameter is an output (<c>out</c>).</summary>
    public Boolean IsOut => ((__Flags >> 16) & 255) == 2;

    /// <summary>Gets whether the parameter takes many arguments (<c>params</c>).</summary>
    public Boolean IsParams => ((__Flags >> 16) & 255) == 3;
}
