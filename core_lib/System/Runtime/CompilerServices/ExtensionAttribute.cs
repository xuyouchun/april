/*
 *  System.dll
 *
 */

////////// ////////// ////////// ////////// //////////

namespace Runtime.CompilerServices;

// Marks an extension method, the class that has it, and the assembly that has the class: the
// compiler writes it, so that the extension methods are found without looking at all the types.
[AttributeUsage]
public class ExtensionAttribute : Attribute
{
    public ExtensionAttribute()
    {

    }
}
