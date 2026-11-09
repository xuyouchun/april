/*
 *  System.dll
 *
 *  Reflection: a property of a type.
 */

namespace Reflection;

////////// ////////// ////////// ////////// //////////

/// <summary>
/// A property of a type, or an indexer (a property that has index parameters).
/// </summary>
[Internal]
public sealed class Property : Member
{
    /// <summary>
    /// Gets the type of the property: the same as <see cref="Member.MemberType"/>.
    /// </summary>
    public Type? PropertyType => this.MemberType;

    /// <summary>
    /// Gets the method that reads the property, or null if it cannot be read.
    /// </summary>
    public Method? GetMethod => Internal.Reflection_GetPropertyGetter(this);

    /// <summary>
    /// Gets the method that writes the property, or null if it cannot be written.
    /// </summary>
    public Method? SetMethod => Internal.Reflection_GetPropertySetter(this);

    /// <summary>Gets whether the property can be read.</summary>
    public Boolean CanRead => this.GetMethod != null;

    /// <summary>Gets whether the property can be written.</summary>
    public Boolean CanWrite => this.SetMethod != null;

    /// <summary>
    /// Gets the value of the property of an object (it calls the method that reads it).
    /// </summary>
    /// <param name="obj">The object; it is ignored for a static property.</param>
    /// <returns>The value, boxed in the way that <see cref="Method.Invoke"/> says.</returns>
    /// <exception cref="InvalidOperationException">
    /// The property cannot be read, or it is an indexer.
    /// </exception>
    /// <exception cref="NullReferenceException">The property is not static and the object is null.</exception>
    /// <exception cref="InvalidCastException">See <see cref="Method.Invoke"/>.</exception>
    public Object? GetValue(Object? obj)
    {
        Method? getter = this.GetMethod;
        if (getter == null || Internal.Reflection_GetParameterCount(this) > 0)
            throw new InvalidOperationException("The property cannot be read.");

        return getter.Invoke(obj, new Object[0]);
    }

    /// <summary>
    /// Sets the value of the property of an object (it calls the method that writes it).
    /// </summary>
    /// <param name="obj">The object; it is ignored for a static property.</param>
    /// <param name="value">The value, boxed in the way that <see cref="Method.Invoke"/> says.</param>
    /// <exception cref="InvalidOperationException">
    /// The property cannot be written, or it is an indexer.
    /// </exception>
    /// <exception cref="NullReferenceException">The property is not static and the object is null.</exception>
    /// <exception cref="InvalidCastException">See <see cref="Method.Invoke"/>.</exception>
    public Void SetValue(Object? obj, Object? value)
    {
        Method? setter = this.SetMethod;
        if (setter == null || Internal.Reflection_GetParameterCount(this) > 0)
            throw new InvalidOperationException("The property cannot be written.");

        Object?[] args = new Object?[1];
        args[0] = value;

        setter.Invoke(obj, args);
    }

    /// <summary>
    /// Gets the index parameters of the property, in the order that they are declared in.
    /// </summary>
    /// <returns>The parameters; an empty array if the property is not an indexer.</returns>
    public Array<Parameter> GetIndexParameters()
    {
        Int32 count = Internal.Reflection_GetParameterCount(this);
        Parameter[] parameters = new Parameter[count];

        for (Int32 k = 0; k < count; k++)
            parameters[k] = Internal.Reflection_GetParameter(this, k);

        return parameters;
    }
}
