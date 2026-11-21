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
    public Exception(String? message)
    {
        this.__message = message;
    }

    // Constructor with message and it's inner exception.
    public Exception(String? message, Exception? innerException)
    {
        this.__message = message;
        this.__innerException = innerException;
    }

    // Exception message.
    public String? Message => __message;

    // Inner exception that caused this exception.
    public Exception? InnerException => __innerException;

    // Exception message.
    private String? __message;

    // Inner exception that caused this exception.
    private Exception? __innerException;
}

////////// ////////// ////////// ////////// //////////

// Base class of system exceptions.
[Internal]
public class SystemException : Exception
{
    // Default constructor.
    public SystemException() { }

    // Constructor with message.
    public SystemException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// NullReferenceException.
[Internal]
public class NullReferenceException : SystemException
{
    // Default constructor.
    public NullReferenceException() { }

    // Constructor with message.
    public NullReferenceException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// InvalidCastException
[Internal]
public class InvalidCastException : SystemException
{
    // Default constructor.
    public InvalidCastException() { }

    // Constructor with message.
    public InvalidCastException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// ArgumentException: an argument of a call is not valid.
public class ArgumentException : SystemException
{
    // Default constructor.
    public ArgumentException() { }

    // Constructor with message.
    public ArgumentException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// FormatException: a format (of a number, of String.Format) or a text that has one is not valid.
public class FormatException : SystemException
{
    // Default constructor.
    public FormatException() { }

    // Constructor with message.
    public FormatException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// InvalidOperationException: an operation is not valid in the state of the object.
public class InvalidOperationException : SystemException
{
    // Default constructor.
    public InvalidOperationException() { }

    // Constructor with message.
    public InvalidOperationException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// CoreType_OutOfRangeException
[Internal]
public class OutOfRangeException : SystemException
{
    // Default constructor.
    public OutOfRangeException() { }

    public OutOfRangeException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// DivideByZeroException: an integer is divided by zero.
[Internal]
public class DivideByZeroException : SystemException
{
    // Default constructor.
    public DivideByZeroException() { }

    public DivideByZeroException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// OverflowException: an arithmetic operation overflows, e.g. MinValue / -1.
[Internal]
public class OverflowException : SystemException
{
    // Default constructor.
    public OverflowException() { }

    public OverflowException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// NotSupportedException: an operation is not supported by the type, e.g. a number is converted to a type that
// does not know it.
public class NotSupportedException : SystemException
{
    // Default constructor.
    public NotSupportedException() { }

    public NotSupportedException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// OperationCanceledException: an operation was canceled (see System.Threading.CancellationToken).
public class OperationCanceledException : SystemException
{
    public OperationCanceledException() { }

    public OperationCanceledException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// ObjectDisposedException: an object that was disposed is used.
public class ObjectDisposedException : InvalidOperationException
{
    public ObjectDisposedException() { }

    public ObjectDisposedException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// TimeoutException: the time of an operation is over.
public class TimeoutException : SystemException
{
    public TimeoutException() { }

    public TimeoutException(String? message) : base(message) { }
}
