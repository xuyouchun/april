/*
 *  System.dll
 *
 *  Exception and the exceptions of the system.
 */

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

// InvalidOperationException: an operation is not valid in the state of the object.
public class InvalidOperationException : SystemException
{
    // Default constructor.
    public InvalidOperationException() { }

    // Constructor with message.
    public InvalidOperationException(String message) : base(message) { }
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

// DivideByZeroException: an integer is divided by zero.
[Internal]
public class DivideByZeroException : SystemException
{
    // Default constructor.
    public DivideByZeroException() { }

    public DivideByZeroException(String message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// OverflowException: an arithmetic operation overflows, e.g. MinValue / -1.
[Internal]
public class OverflowException : SystemException
{
    // Default constructor.
    public OverflowException() { }

    public OverflowException(String message) : base(message) { }
}
