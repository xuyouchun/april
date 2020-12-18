
/*
 *  System.dll
 *
 */

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
};

////////// ////////// ////////// ////////// //////////

[__internal__]
public class Type
{
    // Converts to string.
    public String ToString()
    {
        return "Type";
    }
};

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
    public Int32 Rank { get { return Internal.Array_GetRank(this); } }

    // Returns length of array.
    public Int32 Length { get { return Internal.Array_GetLength(this); } }

    // Returns length of array at specified dimension.
    public Int32 GetLength(Int32 dimension)
    {
        return Internal.Array_GetLengthOfDimension(this, dimension);
    }

};

////////// ////////// ////////// ////////// //////////

[__internal__]
public class Array<T> : Array
{

};

////////// ////////// ////////// ////////// //////////

[__internal__]
public interface ITuple<TItems ...>
{
	Int32 Count { get; }

	Object Get(Int32 index);

	T Get<T>(Int32 index);
};

////////// ////////// ////////// ////////// //////////

[__internal__]
public struct Tuple<TItems ...> : ITuple<TItems ...>
{
	public Tuple(TItems ... items)
	{
		// Internal.Tuple_SetRange(this, (Ptr)items);
	}

	public Object Get(Int32 index)
	{
		return Internal.Object_FromPtr(
			Internal.Tuple_Get(this, index)
		);
	}

	public T Get<T>(Int32 index)
	{
		return (T)Internal.Tuple_Get(this, index);
	}

	public Int32 Count { get { return Internal.Tuple_GetCount(this); } }

};

////////// ////////// ////////// ////////// //////////

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
public class AttributeUsageAttribute : Attribute
{

}

////////// ////////// ////////// ////////// //////////

// Core Compile Attribute
class __955825bf_d13a_4d1c_90d5_478e62ceaab6__
{

};

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

};

////////// ////////// ////////// ////////// //////////


// Delegate.
public class Delegate
{

};

// Delegate.
public class Delegate<TReturn, TArgs ...> : Delegate
{
    // Constructor.
    public Delegate()
    {
        
    }

    public TReturn Invoke(TArgs a)
    {
        return default(TReturn);
    }
};

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
};

////////// ////////// ////////// ////////// //////////

// NullException.
public class NullReferenceException : Exception
{
    // Default constructor.
    public NullReferenceException() { }

    // Constructor with message.
    public NullReferenceException(String message) : base(message) { }
};

////////// ////////// ////////// ////////// //////////


