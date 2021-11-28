
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
        return Internal.Reflection_GetType(this);
    }

    // Converts to string.
    public virtual String ToString()
    {
        return GetType().Name;
    }
};


////////// ////////// ////////// ////////// //////////

[__internal__]
public sealed class Type
{
    public String Name
    {
        get { return Internal.Reflection_GetTypeName(this); }
    }

    public Array<Field> GetFields()
    {
        return new Array<Field>(10);
    }

    public Field GetField(String name)
    {
        return null;
    }

    public Array<Method> GetMethods()
    {
        return new Array<Method>(10);
    }

    public Array<Method> GetMethods(String name)
    {
        return new Array<Method>(10);
    }

    public Method GetMethod(String name, Array<Type> parameterTypes)
    {
        return null;
    }

    public Array<Property> GetProperties()
    {
        return new Array<Property>(10);
    }

    public Property GetProperty(String name)
    {
        return null;
    }

    public Array<Event> GetEvents()
    {
        return new Array<Event>(10);
    }

    public Event GetEvent(String name)
    {
        return null;
    }

    public Array<GenericParameter> GetGenericParameters()
    {
        return new Array<GenericParameter>(10);
    }

    public Assembly Assembly
    {
        get { return null; }
    }

    // Converts to string.
    public String ToString()
    {
        return "Type";
    }
};

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
    public Array(Int32 length)
    {

    }
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
public class Tuple<TItems ...> : ITuple<TItems ...>
{
    public Tuple(TItems ... items)
    {
        Internal.Tuple_SetRange(this, items);
    }

    public Int32 Count { get { return Internal.Tuple_GetCount(this); } }
};

////////// ////////// ////////// ////////// //////////

[__internal__]
public class Enum
{

};

////////// ////////// ////////// ////////// //////////

// Delegate<TReturn, TArgs ...>
[__internal__]
public struct Delegate<TReturn, TArgs ...>
{
    // Constructor.
    public Delegate(Object instance, Method method)
    {
        __instance = instance;
        __method   = method;
    }

    private readonly Object __instance;

    public Object Instance
    {
        get { return __instance; }
    }

    private readonly Method __method;

    public Method Method
    {
        get { return __method; }
    }

    public TReturn Invoke(TArgs ... args)
    {
        Console.WriteLine("Call Invoke");
        return default(TReturn);
    }
};

// MulticastDelegate<TReturn, TArgs ...>
[__internal__]
public sealed class MulticastDelegate<TReturn, TArgs ...>
{
    private readonly Array<Delegate<TReturn, TArgs ...> > __delegates;

    public TReturn Invoke(TArgs ... args)
    {
        return default(TReturn);
    }
};

////////// ////////// ////////// ////////// //////////

// Exception.
[__internal__]
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
[__internal__]
public class NullReferenceException : Exception
{
    // Default constructor.
    public NullReferenceException() { }

    // Constructor with message.
    public NullReferenceException(String message) : base(message) { }
};

////////// ////////// ////////// ////////// //////////


