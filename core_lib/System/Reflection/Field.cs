/*
 *  System.dll
 *
 *  Reflection: a field of a type.
 */

namespace Reflection;

////////// ////////// ////////// ////////// //////////

/// <summary>
/// A field of a type, static or of the instances.
/// </summary>
[Internal]
public sealed class Field : Member
{
    /// <summary>
    /// Gets the type of the field: the same as <see cref="Member.MemberType"/>. For a field of a
    /// generic type that has arguments, it is the type with the arguments (the field <c>T Value</c>
    /// of <c>Box&lt;Int32&gt;</c> is <c>Int32</c>).
    /// </summary>
    public Type? FieldType => this.MemberType;

    /// <summary>Gets whether the field is a constant (<c>const</c>).</summary>
    public Boolean IsConst => (this.Flags & 1024) != 0;

    /// <summary>Gets whether the field is read-only (<c>readonly</c>).</summary>
    public Boolean IsReadOnly => (this.Flags & 2048) != 0;

    /// <summary>
    /// Gets the value of the field of an object.
    /// </summary>
    /// <param name="obj">
    /// The object that has the field, of the type that declares it or of a type that is derived
    /// from it; it is ignored for a static field.
    /// </param>
    /// <returns>
    /// The value: a reference is the object that it is, a number, a struct, an enum and a string
    /// are boxed (a string that is null is null).
    /// </returns>
    /// <exception cref="NullReferenceException">The field is not static and the object is null.</exception>
    /// <exception cref="InvalidCastException">
    /// The object is not of the type that declares the field, or the type of the field is not
    /// known (a field of a generic type that is not made of arguments).
    /// </exception>
    public Object? GetValue(Object? obj)
    {
        __Check(obj, null, 0);
        return Internal.Reflection_GetFieldValue(this, obj);
    }

    /// <summary>
    /// Sets the value of the field of an object.
    /// </summary>
    /// <param name="obj">
    /// The object that has the field, of the type that declares it or of a type that is derived
    /// from it; it is ignored for a static field.
    /// </param>
    /// <param name="value">
    /// The value: for a field of a reference type it is null or an object of the type, for a
    /// field of a number, a struct or an enum it is the box of a value of exactly that type, for
    /// a string it is a string or null.
    /// </param>
    /// <exception cref="NullReferenceException">The field is not static and the object is null.</exception>
    /// <exception cref="InvalidCastException">
    /// The object is not of the type that declares the field, or the value is not of the type of
    /// the field.
    /// </exception>
    public Void SetValue(Object? obj, Object? value)
    {
        __Check(obj, value, 1);
        Internal.Reflection_SetFieldValue(this, obj, value);
    }

    // Throws when the object or the value cannot be used: mode 0 reads the field, mode 1 writes it.
    private Void __Check(Object? obj, Object? value, Int32 mode)
    {
        Int32 code = Internal.Reflection_CheckFieldAccess(this, obj, value, mode);

        if (code == 1)
            throw new NullReferenceException();

        if (code == 2)
            throw new InvalidCastException();
    }
}
