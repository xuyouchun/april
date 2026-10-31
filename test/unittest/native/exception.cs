// try / catch / finally: throw, handlers by type, finally blocks, rethrow.

/* EXPECT

5
zero
deep
cleanup
40
cleanup
negative
body
inner finally
first
outer finally
rethrow
inner
0
f
f
2
f
invalid cast
done

*/

import System;

using System;

class MyError : Exception
{
    public MyError(String message) : base(message) { }
}

class Program
{
    static int Divide(int a, int b)
    {
        if (b == 0)
            throw new Exception("zero");

        return a / b;
    }

    static int Deep(int n)
    {
        if (n == 0)
            throw new Exception("deep");

        return Deep(n - 1) + 1;
    }

    static int WithFinally(int n)
    {
        try
        {
            if (n > 0)
                return n * 10;

            throw new Exception("negative");
        }
        finally
        {
            Console.WriteLine("cleanup");
        }
    }

    static void Rethrower()
    {
        try
        {
            throw new Exception("inner");
        }
        catch (Exception e)
        {
            Console.WriteLine("rethrow");
            throw;
        }
    }

    [EntryPoint]
    public static void Main()
    {
        // Thrown by a callee, caught by the caller.
        try
        {
            Console.WriteLine(Divide(10, 2));
            Console.WriteLine(Divide(1, 0));
            Console.WriteLine("not reached");
        }
        catch (Exception e)
        {
            Console.WriteLine(e.Message);
        }

        // Through several calls.
        try
        {
            Console.WriteLine(Deep(5));
        }
        catch (Exception e)
        {
            Console.WriteLine(e.Message);
        }

        // Finally runs on all the ways out.
        Console.WriteLine(WithFinally(4));

        try
        {
            Console.WriteLine(WithFinally(-1));
        }
        catch (Exception e)
        {
            Console.WriteLine(e.Message);
        }

        // Nested.
        try
        {
            try
            {
                Console.WriteLine("body");
                throw new Exception("first");
            }
            finally
            {
                Console.WriteLine("inner finally");
            }
        }
        catch (Exception e)
        {
            Console.WriteLine(e.Message);
        }
        finally
        {
            Console.WriteLine("outer finally");
        }

        // Rethrow.
        try
        {
            Rethrower();
        }
        catch (Exception e)
        {
            Console.WriteLine(e.Message);
        }

        // A loop that leaves the try blocks.
        for (int k = 0; k < 3; k++)
        {
            try
            {
                if (k == 1)
                    continue;

                Console.WriteLine(k);
            }
            finally
            {
                Console.WriteLine("f");
            }
        }

        // A failed cast.
        object o = "text";
        try
        {
            Program p = (Program)o;
            Console.WriteLine("not reached");
        }
        catch (InvalidCastException e)
        {
            Console.WriteLine("invalid cast");
        }

        Console.WriteLine("done");
    }
}
