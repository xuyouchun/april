// catch (T e) when (condition): the catch runs when the exception is of the type and the condition is true; when it is not, the
// catches that follow are tried.

/* EXPECT

first 1
second 2
general other
none rethrown outer
finally ran
rethrow InvalidOperationException
nested inner caught
same name b
filter once 1

*/

import System;

using System;

class CodeException : Exception
{
    public Int32 Code;

    public CodeException(Int32 code) : base("code") { Code = code; }
}

class Program
{
    static String Handle(Int32 code)
    {
        try
        {
            throw new CodeException(code);
        }
        catch (CodeException e) when (e.Code == 1)
        {
            return "first " + e.Code;
        }
        catch (CodeException e) when (e.Code == 2)
        {
            return "second " + e.Code;
        }
        catch (Exception e)
        {
            return "general other";
        }
    }

    static String Outer()
    {
        try
        {
            try
            {
                throw new CodeException(9);
            }
            catch (CodeException e) when (e.Code == 1)
            {
                return "never";
            }
        }
        catch (Exception e)
        {
            return "none rethrown outer";
        }
    }

    static Int32 Calls = 0;

    static Boolean Filter(Int32 value)
    {
        Calls++;
        return value == 1;
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Handle(1));
        Console.WriteLine(Handle(2));
        Console.WriteLine(Handle(3));
        Console.WriteLine(Outer());

        // The finally block runs.
        String log = "";
        try
        {
            try
            {
                throw new CodeException(5);
            }
            catch (CodeException e) when (e.Code == 5)
            {
                log += "caught ";
            }
            finally
            {
                log += "finally ran";
            }
        }
        catch (Exception)
        {
            log = "wrong";
        }

        Console.WriteLine(log.Substring(log.IndexOf("finally")));

        // throw; inside a catch that has a filter.
        try
        {
            try
            {
                throw new InvalidOperationException("x");
            }
            catch (InvalidOperationException e) when (e.Message == "x")
            {
                throw;
            }
        }
        catch (Exception e)
        {
            Console.WriteLine("rethrow " + e.GetType().Name);
        }

        // A try in a catch.
        try
        {
            throw new CodeException(1);
        }
        catch (CodeException e) when (e.Code == 1)
        {
            try
            {
                throw new InvalidOperationException("inner");
            }
            catch (InvalidOperationException ex) when (ex.Message == "inner")
            {
                Console.WriteLine("nested inner caught");
            }
        }

        // Catches that have the same name for the exception.
        try
        {
            throw new InvalidOperationException("b");
        }
        catch (CodeException e) when (e.Code == 1)
        {
            Console.WriteLine("same name a");
        }
        catch (InvalidOperationException e) when (e.Message == "b")
        {
            Console.WriteLine("same name " + e.Message);
        }

        // The filter is called once.
        try
        {
            throw new CodeException(1);
        }
        catch (CodeException e) when (Filter(e.Code))
        {
        }

        Console.WriteLine("filter once " + Calls);
    }
}
