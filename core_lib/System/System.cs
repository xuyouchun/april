
/*
 *  System.dll
 *
 */

using Reflection;

////////// ////////// ////////// ////////// //////////

[Internal]
public class Object
{
    // Returns the object type.
    public Type GetType()
    {
        return Internal.Reflection_GetType(this);
    }
}

////////// ////////// ////////// ////////// //////////

[Internal]
public sealed class Type
{
    public String Name
    {
        get { return Internal.Reflection_GetTypeName(this); }
    }

    public Array<Field> GetFields()
    {
        return Array.New<Field>(10);
    }

    public Field GetField(String name)
    {
        return null;
    }

    public Array<Method> GetMethods()
    {
        return Array.New<Method>(10);
    }

    public Array<Method> GetMethods(String name)
    {
        return Array.New<Method>(10);
    }

    public Method GetMethod(String name, Array<Type> parameterTypes)
    {
        return null;
    }

    public Array<Property> GetProperties()
    {
        return Array.New<Property>(10);
    }

    public Property GetProperty(String name)
    {
        return null;
    }

    public Array<Event> GetEvents()
    {
        return Array.New<Event>(10);
    }

    public Event GetEvent(String name)
    {
        return null;
    }

    public Array<GenericParameter> GetGenericParameters()
    {
        return Array.New<GenericParameter>(10);
    }

    public Assembly Assembly
    {
        get { return null; }
    }
}

////////// ////////// ////////// ////////// //////////

[Internal]
public abstract class Array : Object
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

    public static Array<T> New<T>(Int32 length)
    {
        return default(Array<T>);
    }
}

////////// ////////// ////////// ////////// //////////

[Internal]
public abstract class Array<T> : Array
{
    // Gets/Sets value of specified index.
    public T this[Int32 index]
    {
        get
        {
            return Internal.Array_GetValue<T>(index, this);
        }
        set
        {
            Internal.Array_SetValue<T>(value, index, this);
        }
    }
}

////////// ////////// ////////// ////////// //////////

[Internal]
public interface ITuple<TItems ...>
{
    Int32 Count { get; }

    Object Get(Int32 index);

    T Get<T>(Int32 index);
}

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Tuple<TItems ...> : ITuple<TItems ...>
{
    using __SelfType = Tuple<TItems ...>;

    public Tuple(TItems ... items)
    {
        Internal.Tuple_SetRange(typeof(__SelfType), this, items);
    }

    public Int32 Count
    {
        get { return Internal.Tuple_GetCount(typeof(__SelfType)); }
    }

    public Object Get(Int32 index)
    {
        return null;
    }

    public T Get<T>(Int32 index)
    {
        return default(T);
    }
}

////////// ////////// ////////// ////////// //////////

[Internal]
public class Enum
{

}

////////// ////////// ////////// ////////// //////////

// Delegate<TReturn, TArgs ...>
[Internal]
public struct Delegate<TReturn, TArgs ...>
{
    // Constructor.
    private Delegate(Object instance, Method method)
    {
        Internal.Delegate_Init();
    }

    private Delegate(Object instance, Method method, Int32 callType)
    {
        Internal.Delegate_InitWithCallType();
    }

    private readonly Object __instance;
    private readonly Method __method;

    public TReturn Invoke(TArgs ... args)
    {
        return Internal.Delegate_Invoke<TReturn>(); 
    }
}

// MulticastDelegate<TReturn, TArgs ...>
[Internal]
public sealed class MulticastDelegate<TReturn, TArgs ...>
{
    private MulticastDelegate() { }

    public using Self = MulticastDelegate<TReturn, TArgs ...>;
    public using Delegate = Delegate<TReturn, TArgs ...>;

    private readonly Array<Delegate> __delegates;

    public Int32 Count
    {
        get { return Internal.MulticastDelegate_GetCount(); }
    }

    public Delegate this[Int32 index]
    {
        get { return Internal.MulticastDelegate_Get<Delegate>(index); }
    }

    /*
    public static Self Add(Delegate func)
    {
        return MulticastDelegate_Add<Self>(this, func);
    }

    public static Self Remove(Delegate func)
    {
        return MulticastDelegate_Remove<Self>(this, func);
    }
    */

    public TReturn Invoke(TArgs ... args)
    {
        return Internal.Delegate_Invoke<TReturn>(); 
    }
}

////////// ////////// ////////// ////////// //////////

// Exception.
[Internal]
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

// Converts to string.
public interface IString
{
    String ToString();
}

// Get hash code.
public interface IHashCode
{
    Int32 GetHashCode();
}

// Check if two objects are equal.
public interface IEquals
{
    Boolean Equals(Object obj);
}

////////// ////////// ////////// ////////// //////////

// Base class of system exceptions.
[Internal]
public class SystemException : Exception
{
    // Default constructor.
    public SystemException() { }

    // Constructor with message.
    public SystemException(String message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// NullReferenceException.
[Internal]
public class NullReferenceException : SystemException
{
    // Default constructor.
    public NullReferenceException() { }

    // Constructor with message.
    public NullReferenceException(String message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// InvalidCastException
[Internal]
public class InvalidCastException : SystemException
{
    // Default constructor.
    public InvalidCastException() { }

    // Constructor with message.
    public InvalidCastException(String message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// CoreType_OutOfRangeException
[Internal]
public class OutOfRangeException : SystemException
{
    // Default constructor.
    public OutOfRangeException() { }

    public OutOfRangeException(String message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

