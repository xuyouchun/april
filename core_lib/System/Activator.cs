/*
 *  System.dll
 *
 *  Activator: makes the objects of a type that is known when the program runs.
 */

using Reflection;

////////// ////////// ////////// ////////// //////////

/// <summary>
/// Makes objects of a type that is a generic argument: <c>new T()</c> of a param that has the
/// <c>new()</c> constraint is <c>Activator.CreateInstance&lt;T&gt;()</c> (the compiler writes it).
/// </summary>
public static class Activator
{
    /// <summary>
    /// Makes an object of the type, with its constructor that has no parameters.
    /// </summary>
    /// <typeparam name="T">The type: a class that has a constructor without parameters, or a struct.</typeparam>
    /// <returns>
    /// The new object of a class; the default value of a struct, a number or a string (the
    /// constructor of a struct that is declared is not run).
    /// </returns>
    /// <exception cref="InvalidOperationException">The class has no constructor without parameters.</exception>
    public static T CreateInstance<T>()
    {
        Type type = typeof(T);

        if (type.IsClass)
        {
            Array<Method> methods = type.GetMethods();

            for (Int32 k = 0; k < methods.Length; k++)
            {
                Method method = methods[k];

                if (method.IsConstructor && method.GetParameters().Length == 0)
                    return (T)method.NewInstance(new Object?[0]);
            }

            throw new InvalidOperationException("The type has no constructor without parameters.");
        }

        return default(T);
    }
}
