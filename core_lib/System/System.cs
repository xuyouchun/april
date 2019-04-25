
/*
 *  Types
 *
 */

////////// ////////// ////////// ////////// //////////

[__internal__]
class Object
{

};

////////// ////////// ////////// ////////// //////////

[__internal__]
public class Attribute
{

}

////////// ////////// ////////// ////////// //////////

[__internal__]
class Array : Object
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
};

////////// ////////// ////////// ////////// //////////

[__internal__]
class Array<T> : Array
{

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
    public InlineAttribute()
    {

    }

    public InlineAttribute(Boolean value)
    {

    }
}

////////// ////////// ////////// ////////// //////////

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
class EntryPointAttribute
{

};


////////// ////////// ////////// ////////// //////////

// Exception.
class Exception
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
    private String    __message;

    // Inner exception that caused this exception.
    private Exception __innerException;
};







