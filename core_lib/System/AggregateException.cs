/*
 *  System.dll
 *
 *  AggregateException: many exceptions that were thrown together (the tasks, the parallel loops).
 */

using Collections;

////////// ////////// ////////// ////////// //////////

// Holds the exceptions that more than one operation threw.
public class AggregateException : Exception
{
    private Array<Exception> __exceptions;

    public AggregateException(Exception innerException) : base(__MessageOf(__ArrayOf(innerException)), innerException)
    {
        __exceptions = __ArrayOf(innerException);
    }

    public AggregateException(Array<Exception> innerExceptions) : base(__MessageOf(innerExceptions), __FirstOf(innerExceptions))
    {
        __exceptions = innerExceptions;
    }

    public AggregateException(String? message, Array<Exception> innerExceptions) : base(message, __FirstOf(innerExceptions))
    {
        __exceptions = innerExceptions;
    }

    // The exceptions, in the order they were thrown.
    public Array<Exception> InnerExceptions => __exceptions;

    // The exceptions that are not aggregate ones: the aggregate exceptions that it holds are opened.
    public AggregateException Flatten()
    {
        List<Exception> all = new List<Exception>();
        __Collect(this, all);

        return new AggregateException(all.ToArray());
    }

    private static Void __Collect(AggregateException source, List<Exception> output)
    {
        Array<Exception> items = source.__exceptions;

        for (Int32 k = 0; k < items.Length; k++)
        {
            AggregateException? inner = items[k] as AggregateException;

            if (inner != null)
                __Collect(inner, output);
            else
                output.Add(items[k]);
        }
    }

    private static Array<Exception> __ArrayOf(Exception exception)
    {
        Exception[] items = new Exception[1];
        items[0] = exception;

        return items;
    }

    private static Exception? __FirstOf(Array<Exception> exceptions)
    {
        return exceptions != null && exceptions.Length > 0? exceptions[0] : null;
    }

    private static String __MessageOf(Array<Exception> exceptions)
    {
        String text = "One or more errors occurred.";

        if (exceptions != null)
        {
            for (Int32 k = 0; k < exceptions.Length; k++)
            {
                String? message = exceptions[k].Message;
                text = text + " (" + (message == null? "" : message) + ")";
            }
        }

        return text;
    }
}
