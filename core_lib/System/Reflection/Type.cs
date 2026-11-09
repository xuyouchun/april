/*
 *  System.dll
 *
 *  Reflection: the type of an object.
 */

namespace Reflection;

////////// ////////// ////////// ////////// //////////

/// <summary>
/// A type: a class, a struct, an interface or an enum, with the arguments of a generic type.
/// It is the type of an object (<see cref="GetType"/>), or the type that <c>typeof</c> gives.
/// </summary>
/// <remarks>
/// A type is a handle of the tables of the runtime, not an object of the heap: there is one for
/// each type, so two types are the same if they are the same object (<see cref="Equals"/>).
/// </remarks>
[Internal]
public sealed class Type : ReflectionInfo
{
    // The flags of the type: the access and the modifiers (see Member) in the bits 0 to 15, and its
    // ttype_t (1 class, 2 struct, 3 interface, 4 enum) in the bits 32 to 35.
    private Int64 __Flags => Internal.Reflection_GetTypeFlags(this);

    private Int64 __TType => (__Flags >> 32) & 15;

    /// <summary>
    /// Gets the type of an object.
    /// </summary>
    /// <param name="obj">The object, that is not null.</param>
    /// <returns>The type of the object: the type that it was made with, not the static type.</returns>
    public static Type GetType(Object obj) => Internal.Reflection_GetType(obj);

    /// <summary>
    /// Gets whether a type is this type.
    /// </summary>
    /// <param name="obj">The other type, or null.</param>
    /// <returns>True if they are the same type.</returns>
    public Boolean Equals(Type? obj) => Internal.Object_ReferenceEquals(this, obj);

    /// <summary>
    /// Gets the name of the type with its namespace, its host types and the arguments of a
    /// generic type: <c>Collections.Dictionary&lt;String,Int32&gt;</c>. The types of the core
    /// library are not in a namespace (<c>Int32</c>).
    /// </summary>
    public String FullName => Internal.Reflection_GetTypeFullName(this);

    /// <summary>
    /// Gets the base type of a class, or null if it has none (<c>Object</c>, structs, interfaces).
    /// </summary>
    public Type? BaseType => Internal.Reflection_GetTypeBase(this);

    /// <summary>
    /// Gets the assembly that the type is defined in.
    /// </summary>
    public Assembly? Assembly => Internal.Reflection_GetTypeAssembly(this);

    /// <summary>Gets whether the type is a class.</summary>
    public Boolean IsClass => __TType == 1;

    /// <summary>Gets whether the type is a struct.</summary>
    public Boolean IsStruct => __TType == 2;

    /// <summary>Gets whether the type is an interface.</summary>
    public Boolean IsInterface => __TType == 3;

    /// <summary>Gets whether the type is an enum.</summary>
    public Boolean IsEnum => __TType == 4;

    /// <summary>Gets whether the type is abstract.</summary>
    public Boolean IsAbstract => (__Flags & 256) != 0;

    /// <summary>Gets whether the type is sealed.</summary>
    public Boolean IsSealed => (__Flags & 32) != 0;

    /// <summary>
    /// Gets whether the type is a generic type: it has generic parameters, or it is made with
    /// the arguments of them.
    /// </summary>
    public Boolean IsGenericType => Internal.Reflection_GetMemberCount(this, 4) > 0;

    /// <summary>
    /// Gets the types that the type declares: the classes that are nested in it, the ones that
    /// the compiler makes too (the state of an iterator, the environment of a lambda). Those of
    /// an instance of a generic type are not listed.
    /// </summary>
    /// <returns>The types in the order that they are declared in; an empty array if there is none.</returns>
    public Array<Type> GetNestedTypes()
    {
        Int32 count = Internal.Reflection_GetNestedTypeCount(this);
        Type[] types = new Type[count];

        for (Int32 k = 0; k < count; k++)
            types[k] = Internal.Reflection_GetNestedType(this, k);

        return types;
    }

    /// <summary>
    /// Gets the fields that the type declares: the static ones and the ones of the instances, of
    /// any access. The fields of the base types are not included.
    /// </summary>
    /// <returns>The fields in the order that they are declared in; an empty array if there is none.</returns>
    public Array<Field> GetFields()
    {
        Int32 count = Internal.Reflection_GetMemberCount(this, 0);
        Field[] fields = new Field[count];

        for (Int32 k = 0; k < count; k++)
            fields[k] = Internal.Reflection_GetField(this, k);

        return fields;
    }

    /// <summary>
    /// Gets the field of a name that the type declares.
    /// </summary>
    /// <param name="name">The name of the field.</param>
    /// <returns>The field, or null if there is none.</returns>
    public Field? GetField(String name)
    {
        Int32 count = Internal.Reflection_GetMemberCount(this, 0);

        for (Int32 k = 0; k < count; k++)
        {
            Field field = Internal.Reflection_GetField(this, k);
            if (field.Name == name)
                return field;
        }

        return null;
    }

    /// <summary>
    /// Gets the methods that the type declares, the constructors and the accessors of properties
    /// and events too. The methods of the base types are not included.
    /// </summary>
    /// <returns>The methods in the order that they are declared in; an empty array if there is none.</returns>
    public Array<Method> GetMethods()
    {
        Int32 count = Internal.Reflection_GetMemberCount(this, 1);
        Method[] methods = new Method[count];

        for (Int32 k = 0; k < count; k++)
            methods[k] = Internal.Reflection_GetMethod(this, k);

        return methods;
    }

    /// <summary>
    /// Gets the methods of a name that the type declares (the overloads of it).
    /// </summary>
    /// <param name="name">The name of the methods.</param>
    /// <returns>The methods; an empty array if there is none.</returns>
    public Array<Method> GetMethods(String name)
    {
        Int32 count = Internal.Reflection_GetMemberCount(this, 1);
        Int32 matched = 0;

        for (Int32 k = 0; k < count; k++)
        {
            if (Internal.Reflection_GetMethod(this, k).Name == name)
                matched++;
        }

        Method[] methods = new Method[matched];
        Int32 index = 0;

        for (Int32 k = 0; k < count; k++)
        {
            Method method = Internal.Reflection_GetMethod(this, k);
            if (method.Name == name)
                methods[index++] = method;
        }

        return methods;
    }

    /// <summary>
    /// Gets the method of a name that has parameters of types.
    /// </summary>
    /// <param name="name">The name of the method.</param>
    /// <param name="parameterTypes">The types of the parameters, in order; empty for a method that has none.</param>
    /// <returns>The method, or null if there is none.</returns>
    public Method? GetMethod(String name, Array<Type> parameterTypes)
    {
        Int32 count = Internal.Reflection_GetMemberCount(this, 1);

        for (Int32 k = 0; k < count; k++)
        {
            Method method = Internal.Reflection_GetMethod(this, k);
            if (method.Name != name)
                continue;

            Array<Parameter> parameters = method.GetParameters();
            if (parameters.Length != parameterTypes.Length)
                continue;

            Boolean same = true;
            for (Int32 i = 0; i < parameters.Length && same; i++)
            {
                Type? parameterType = parameters[i].ParameterType;
                same = parameterType != null && parameterType.Equals(parameterTypes[i]);
            }

            if (same)
                return method;
        }

        return null;
    }

    /// <summary>
    /// Gets the properties that the type declares (the indexers too). The properties of the base
    /// types are not included.
    /// </summary>
    /// <returns>The properties in the order that they are declared in; an empty array if there is none.</returns>
    public Array<Property> GetProperties()
    {
        Int32 count = Internal.Reflection_GetMemberCount(this, 2);
        Property[] properties = new Property[count];

        for (Int32 k = 0; k < count; k++)
            properties[k] = Internal.Reflection_GetProperty(this, k);

        return properties;
    }

    /// <summary>
    /// Gets the property of a name that the type declares.
    /// </summary>
    /// <param name="name">The name of the property.</param>
    /// <returns>The property, or null if there is none.</returns>
    public Property? GetProperty(String name)
    {
        Int32 count = Internal.Reflection_GetMemberCount(this, 2);

        for (Int32 k = 0; k < count; k++)
        {
            Property property = Internal.Reflection_GetProperty(this, k);
            if (property.Name == name)
                return property;
        }

        return null;
    }

    /// <summary>
    /// Gets the events that the type declares. The events of the base types are not included.
    /// </summary>
    /// <returns>The events in the order that they are declared in; an empty array if there is none.</returns>
    public Array<Event> GetEvents()
    {
        Int32 count = Internal.Reflection_GetMemberCount(this, 3);
        Event[] events = new Event[count];

        for (Int32 k = 0; k < count; k++)
            events[k] = Internal.Reflection_GetEvent(this, k);

        return events;
    }

    /// <summary>
    /// Gets the event of a name that the type declares.
    /// </summary>
    /// <param name="name">The name of the event.</param>
    /// <returns>The event, or null if there is none.</returns>
    public Event? GetEvent(String name)
    {
        Int32 count = Internal.Reflection_GetMemberCount(this, 3);

        for (Int32 k = 0; k < count; k++)
        {
            Event e = Internal.Reflection_GetEvent(this, k);
            if (e.Name == name)
                return e;
        }

        return null;
    }

    /// <summary>
    /// Gets the generic parameters of the type, in the order that they are declared in. A type
    /// that is made with arguments has the parameters of the generic type that it is made from.
    /// </summary>
    /// <returns>The generic parameters; an empty array if the type is not generic.</returns>
    public Array<GenericParameter> GetGenericParameters()
    {
        Int32 count = Internal.Reflection_GetMemberCount(this, 4);
        GenericParameter[] parameters = new GenericParameter[count];

        for (Int32 k = 0; k < count; k++)
            parameters[k] = Internal.Reflection_GetTypeGenericParameter(this, k);

        return parameters;
    }
}
