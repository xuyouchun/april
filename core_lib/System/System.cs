
/*
 *  System.dll
 *
 */

using Reflection;

////////// ////////// ////////// ////////// //////////

[__internal__]
public class Object
{
    // Returns the object type.
    public Type GetType()
    {
        return new Type();
    }

    // Converts to string.
    public String ToString()
    {
        return "Object";
        //return GetType().ToString();
    }
}

////////// ////////// ////////// ////////// //////////

[__internal__]
public class Type
{
    // Converts to string.
    public String ToString()
    {
        return "Type";
    }
}

////////// ////////// ////////// ////////// //////////

[__internal__]
public class Attribute
{

}

////////// ////////// ////////// ////////// //////////

[__internal__]
public class Array : Object
{
    // Returns rank of array.
    public Int32 Rank { get { return Array_GetRank(this); } }

    // Returns length of array.
    public Int32 Length { get { return Array_GetLength(this); } }

    // Returns length of array at specified dimension.
    public Int32 GetLength(Int32 dimension)
    {
        return Array_GetLengthOfDimension(this, dimension);
    }

    [__internal__]
    private static extern Int32 Array_GetRank(Array array);

    [__internal__]
    private static extern Int32 Array_GetLength(Array array);

    [__internal__]
    private static extern Int32 Array_GetLengthOfDimension(Array array, Int32 dimension);
}

////////// ////////// ////////// ////////// //////////

[__internal__]
public class Array<T> : Array
{

}

////////// ////////// ////////// ////////// //////////

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
public class AttributeUsageAttribute : Attribute
{

}

////////// ////////// ////////// ////////// //////////

// Core Compile Attribute
class __955825bf_d13a_4d1c_90d5_478e62ceaab6__
{

}

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
class __internal__ : Attribute
{

}

////////// ////////// ////////// ////////// //////////

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
public class InlineAttribute : Attribute
{
    // Constructor.
    public InlineAttribute()
    {

    }

    // Constructor.
    public InlineAttribute(Boolean value)
    {

    }
}

////////// ////////// ////////// ////////// //////////

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
public class EntryPointAttribute
{

}

////////// ////////// ////////// ////////// //////////


// Delegate.
public class Delegate
{

    // Creates new delegate.
    public static Delegate<TReturn, params TArgs>
        Create<TReturn, params TArgs>(Reflection.MethodInfo methodInfo, Object firstArgument)
    {
        //return new Delegate<TReturn, params TArgs>(methodInfo, firstArgument);
        return null;
    }
}

// Delegate.
public class Delegate<TReturn, params TArgs> : Delegate
{
    // Constructor.
    public Delegate(Reflection.MethodInfo methodInfo, Object firstArgument)
    {

    }

    public TReturn Invoke(TArgs a)
    {
        return default(TReturn);
    }
}

/*
// MulticastDelegate
public class MulticastDelegate<TReturn, params TArgs> : Delegate<TReturn, params TArgs>
{
    
}
*/

////////// ////////// ////////// ////////// //////////

// Exception.
public class Exception
{
    // Default constructor.
    public Exception() { }

    // Constructor with message.
    public Exception(String message)
    {
        this.__message = message;
    }

    // Constructor with message and it's inner exception.
    public Exception(String message, Exception innerException)
    {
        this.__message = message;
        this.__innerException = innerException;
    }

    // Exception message.
    public String Message
    {
        get { return __message; }
    }

    // Inner exception that caused this exception.
    public Exception InnerException
    {
        get { return __innerException; }
    }

    // Exception message.
    private String __message;

    // Inner exception that caused this exception.
    private Exception __innerException;
}

////////// ////////// ////////// ////////// //////////

// NullException.
public class NullReferenceException : Exception
{
    // Default constructor.
    public NullReferenceException() { }

    // Constructor with message.
    public NullReferenceException(String message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////


