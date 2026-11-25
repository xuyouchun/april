/*
 *  System.dll
 *
 *  Reflection: a method of a type.
 */

namespace Reflection;

////////// ////////// ////////// ////////// //////////

/// <summary>
/// A method of a type: a method, a constructor, a static constructor, or the accessor of a
/// property or of an event.
/// </summary>
[Internal]
public sealed class Method : Member
{
    // The trait of the method (method_trait_t): 1 normal, 2 constructor, 3 static constructor,
    // 4 destructor. It is in the bits 16 to 23 of the flags.
    private Int64 __Trait => (this.Flags >> 16) & 255;

    /// <summary>
    /// Gets the return type of the method: the same as <see cref="Member.MemberType"/>. It is
    /// <c>Void</c> for a method that returns nothing.
    /// </summary>
    public Type? ReturnType => this.MemberType;

    /// <summary>Gets whether the method is a constructor of instances.</summary>
    public Boolean IsConstructor => __Trait == 2;

    /// <summary>Gets whether the method is the static constructor of its type.</summary>
    public Boolean IsStaticConstructor => __Trait == 3;

    /// <summary>Gets whether the method has generic parameters (<c>T Get&lt;T&gt;()</c>).</summary>
    public Boolean IsGenericMethod => Internal.Reflection_GetGenericParameterCount(this) > 0;

    /// <summary>
    /// Gets the parameters of the method, in the order that they are declared in.
    /// </summary>
    /// <returns>The parameters; an empty array if the method has none.</returns>
    public Array<Parameter> GetParameters()
    {
        Int32 count = Internal.Reflection_GetParameterCount(this);
        Parameter[] parameters = new Parameter[count];

        for (Int32 k = 0; k < count; k++)
            parameters[k] = Internal.Reflection_GetParameter(this, k);

        return parameters;
    }

    /// <summary>
    /// Calls the method.
    /// </summary>
    /// <param name="target">
    /// The object that the method is called on (a virtual method calls the implementation of the
    /// type of the object); it is ignored for a static method.
    /// </param>
    /// <param name="args">
    /// The arguments, in the order of the parameters: for a parameter of a reference type, null or
    /// an object of the type; for a parameter of a number, an enum, a bool or a char, the box of a
    /// value of exactly that type; for a string, a string or null. It can be null when the method
    /// has no parameters.
    /// </param>
    /// <returns>
    /// What the method returns: a reference is the object, a number, an enum, a bool, a char and a
    /// string are boxed (a string that is null is null); null for a method of <c>void</c>.
    /// </returns>
    /// <exception cref="ArgumentException">The count of the arguments is not that of the parameters.</exception>
    /// <exception cref="InvalidOperationException">
    /// The method is a static constructor, or is generic, or has a parameter that is <c>ref</c>,
    /// <c>out</c> or <c>params</c>.
    /// </exception>
    /// <exception cref="NullReferenceException">The method is not static and the target is null.</exception>
    /// <exception cref="InvalidCastException">
    /// The target is not of the type that declares the method, or an argument is not of the type of
    /// its parameter, or the method takes or returns a struct, or the target is a struct or a
    /// number (the methods of the values are not supported yet).
    /// </exception>
    public Object? Invoke(Object? target, Array<Object?> args)
    {
        if (this.IsStaticConstructor || this.IsGenericMethod)
            throw new InvalidOperationException("The method cannot be invoked.");

        Array<Parameter> parameters = this.GetParameters();
        Int32 count = args == null? 0 : args.Length;

        if (count != parameters.Length)
            throw new ArgumentException("The count of the arguments is not the count of the parameters.");

        for (Int32 k = 0; k < count; k++)
        {
            if (parameters[k].IsRef || parameters[k].IsOut || parameters[k].IsParams)
                throw new InvalidOperationException("A parameter that is ref, out or params is not supported.");
        }

        // The constructor of Object has nothing to run: the native back ends make no function for it.
        if (this.IsConstructor && this.DeclaringType.Equals(typeof(Object)))
        {
            if (target == null)
                throw new NullReferenceException();

            return null;
        }

        Int32 code = Internal.Reflection_CheckInvoke(this, target, args);

        if (code == 1)
            throw new NullReferenceException();

        if (code == 2)
            throw new InvalidCastException();

        return Internal.Reflection_Invoke(this, target, args);
    }

    /// <summary>
    /// Makes an object of the class that has the constructor, and runs the constructor.
    /// </summary>
    /// <param name="args">The arguments of the constructor, as <see cref="Invoke"/> says.</param>
    /// <returns>The new object.</returns>
    /// <exception cref="InvalidOperationException">The method is not a constructor of a class.</exception>
    /// <exception cref="ArgumentException">The count of the arguments is not that of the parameters.</exception>
    /// <exception cref="InvalidCastException">An argument is not of the type of its parameter.</exception>
    public Object NewInstance(Array<Object?> args)
    {
        if (!this.IsConstructor || !this.DeclaringType.IsClass)
            throw new InvalidOperationException("The method is not the constructor of a class.");

        Object obj = Internal.Reflection_NewObject(this.DeclaringType);
        this.Invoke(obj, args);

        return obj;
    }

    /// <summary>
    /// Gets the generic parameters of the method, in the order that they are declared in.
    /// </summary>
    /// <returns>The generic parameters; an empty array if the method is not generic.</returns>
    public Array<GenericParameter> GetGenericParameters()
    {
        Int32 count = Internal.Reflection_GetGenericParameterCount(this);
        GenericParameter[] parameters = new GenericParameter[count];

        for (Int32 k = 0; k < count; k++)
            parameters[k] = Internal.Reflection_GetGenericParameter(this, k);

        return parameters;
    }
}
