/*
 *  System.dll
 *
 */

////////// ////////// ////////// ////////// //////////

namespace Runtime.CompilerServices;

// Marks what the compiler makes: the classes, the fields, the methods and the properties of the
// iterators, of the lambdas and of the automatic properties. It is an attribute like the others:
// the reflection does not treat what has it in another way.
[AttributeUsage]
public class CompilerGeneratedAttribute : Attribute
{
    public CompilerGeneratedAttribute()
    {

    }
}
