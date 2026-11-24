/*
 *  System.dll
 *
 *  ThrowHelper: the throw expression (x ?? throw e, c? x : throw e) is a call of it.
 */

////////// ////////// ////////// ////////// //////////

/// <summary>
/// Throws the exception where an expression is wanted: <c>x ?? throw e</c> is <c>x ?? ThrowHelper.Throw&lt;T&gt;(e)</c>
/// (the compiler writes it, T is the type of x).
/// </summary>
public static class ThrowHelper
{
    /// <summary>Throws the exception, it returns nothing: it is a value of the type T that is never made.</summary>
    /// <typeparam name="T">The type of the value that the expression stands for.</typeparam>
    /// <param name="exception">The exception.</param>
    /// <returns>Nothing: the method throws.</returns>
    public static T Throw<T>(Exception exception)
    {
        throw exception;
    }
}
