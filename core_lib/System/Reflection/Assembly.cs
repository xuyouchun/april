/*
 *  System.dll
 *
 *  Reflection: an assembly.
 */

namespace Reflection;

////////// ////////// ////////// ////////// //////////

/// <summary>
/// An assembly: the file of the code that a type is defined in. The name of the assembly is
/// <see cref="ReflectionInfo.Name"/>.
/// </summary>
/// <remarks>
/// The types of an assembly cannot be listed yet: a native program does not keep the types that it
/// does not use.
/// </remarks>
[Internal]
public sealed class Assembly : ReflectionInfo
{

}
