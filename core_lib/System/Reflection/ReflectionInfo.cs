/*
 *  System.dll
 *
 *  Reflection: the base of the objects of reflection.
 */

namespace Reflection;

////////// ////////// ////////// ////////// //////////

/// <summary>
/// The base of the objects of reflection: types, assemblies, members, parameters and generic
/// parameters. All of them have a name and attributes.
/// </summary>
/// <remarks>
/// The objects of reflection are handles of the tables of the runtime, they are not objects of
/// the heap: they are never collected, and two objects that stand for the same thing are the same
/// object (<c>type.GetField("x") == type.GetField("x")</c> is true).
/// </remarks>
[Internal]
public abstract class ReflectionInfo
{
    /// <summary>
    /// Gets the name of the object: the name of a member, the name of a parameter, the name of an
    /// assembly, the name of a type (with the names of the parameters of a generic type:
    /// <c>Dictionary&lt;TKey,TValue&gt;</c>; a nested type has the name of its host type before its
    /// own). It does not have the namespace, or the host type of a member.
    /// </summary>
    public String Name => Internal.Reflection_IsMember(this) != 0? Internal.Reflection_GetInfoName(this)
                                                                   : Internal.Reflection_GetTypeName(this);

    /// <summary>
    /// Gets the attributes of the object: the ones of a type, a field, a method, a property, an
    /// event, a parameter, a generic parameter or an assembly. Each of them is made with the
    /// constructor and the arguments that were written, as <c>[Author("ann", 3)]</c>; the named
    /// arguments (<c>[Author("ann", Level = 3)]</c>) assign the fields and the properties of the
    /// attribute after that.
    /// </summary>
    /// <returns>The attributes, in the order that they were written; empty if there is none.</returns>
    public Array<Attribute> GetAttributes()
    {
        Boolean member = Internal.Reflection_IsMember(this) != 0;

        Int32 count = member? Internal.Reflection_GetAttributeCount(this)
                            : Internal.Reflection_GetTypeAttributeCount(this);
        Attribute[] attributes = new Attribute[count];

        for (Int32 k = 0; k < count; k++)
        {
            Method constructor = member? Internal.Reflection_GetAttributeConstructor(this, k)
                                       : Internal.Reflection_GetTypeAttributeConstructor(this, k);
            Int32 length = member? Internal.Reflection_GetAttributeArgumentCount(this, k)
                                 : Internal.Reflection_GetTypeAttributeArgumentCount(this, k);
            Object?[] args = new Object?[length];

            for (Int32 i = 0; i < length; i++)
            {
                args[i] = member? Internal.Reflection_GetAttributeArgument(this, k, i)
                                : Internal.Reflection_GetTypeAttributeArgument(this, k, i);
            }

            Attribute attribute = (Attribute)constructor.NewInstance(args);

            // The named arguments assign the fields and the properties.
            Int32 named = member? Internal.Reflection_GetAttributeNamedCount(this, k)
                                : Internal.Reflection_GetTypeAttributeNamedCount(this, k);

            for (Int32 i = 0; i < named; i++)
            {
                String name = member? Internal.Reflection_GetAttributeNamedName(this, k, i)
                                    : Internal.Reflection_GetTypeAttributeNamedName(this, k, i);
                Object? value = member? Internal.Reflection_GetAttributeNamedValue(this, k, i)
                                      : Internal.Reflection_GetTypeAttributeNamedValue(this, k, i);

                __Assign(attribute, name, value);
            }

            attributes[k] = attribute;
        }

        return attributes;
    }

    // Assigns the field or the property of an attribute that has the name.
    private static Void __Assign(Object obj, String name, Object? value)
    {
        for (Type? t = Type.GetType(obj); t != null; t = t.BaseType)
        {
            Field? field = t.GetField(name);
            if (field != null)
            {
                field.SetValue(obj, value);
                return;
            }

            Property? property = t.GetProperty(name);
            if (property != null)
            {
                property.SetValue(obj, value);
                return;
            }
        }
    }

    /// <summary>
    /// Gets the attributes of the object that are of a type (or of a type that is derived from it).
    /// </summary>
    /// <param name="attributeType">The type of the attributes.</param>
    /// <returns>The attributes, in the order that they were written; empty if there is none.</returns>
    public Array<Attribute> GetAttributes(Type attributeType)
    {
        Array<Attribute> attributes = this.GetAttributes();
        Int32 found = 0;

        for (Int32 k = 0; k < attributes.Length; k++)
        {
            if (__IsOfType(attributes[k], attributeType))
                found++;
        }

        Attribute[] result = new Attribute[found];
        found = 0;

        for (Int32 k = 0; k < attributes.Length; k++)
        {
            if (__IsOfType(attributes[k], attributeType))
                result[found++] = attributes[k];
        }

        return result;
    }

    private static Boolean __IsOfType(Object obj, Type type)
    {
        for (Type? t = Type.GetType(obj); t != null; t = t.BaseType)
        {
            if (t.Equals(type))
                return true;
        }

        return false;
    }
}
