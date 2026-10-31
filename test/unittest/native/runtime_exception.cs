// The exceptions that the runtime raises: out of range, null reference, invalid cast.

/* EXPECT

0
out of range
out of range 2
9
out of range 3
null array
null shape
null unbox
system exception
exception
finally
outer handler
done

*/

import System;

using System;

class Shape
{
    public virtual int Area()
    {
        return 1;
    }
}

class Program
{
    static int Read(int[] values, int index)
    {
        return values[index];
    }

    [EntryPoint]
    public static void Main()
    {
        int[] a = new int[3];

        // Out of range, read and write.
        try
        {
            Console.WriteLine(Read(a, 1));
            Console.WriteLine(Read(a, 3));
            Console.WriteLine("not reached");
        }
        catch (OutOfRangeException e)
        {
            Console.WriteLine("out of range");
        }

        try
        {
            a[-1] = 5;
        }
        catch (OutOfRangeException e)
        {
            Console.WriteLine("out of range 2");
        }

        // Two dimensions.
        int[,] m = new int[2, 2];
        try
        {
            m[1, 1] = 9;
            Console.WriteLine(m[1, 1]);
            m[1, 2] = 9;
        }
        catch (OutOfRangeException e)
        {
            Console.WriteLine("out of range 3");
        }

        // Null references.
        int[] nothing = null;
        try
        {
            Console.WriteLine(Read(nothing, 0));
        }
        catch (NullReferenceException e)
        {
            Console.WriteLine("null array");
        }

        Shape shape = null;
        try
        {
            Console.WriteLine(shape.Area());
        }
        catch (NullReferenceException e)
        {
            Console.WriteLine("null shape");
        }

        object o = null;
        try
        {
            int value = (int)o;
            Console.WriteLine("not reached");
        }
        catch (NullReferenceException e)
        {
            Console.WriteLine("null unbox");
        }

        // They are system exceptions, and can be caught as Exception.
        try
        {
            Console.WriteLine(Read(a, 10));
        }
        catch (SystemException e)
        {
            Console.WriteLine("system exception");
        }

        try
        {
            Console.WriteLine(Read(a, 10));
        }
        catch (Exception e)
        {
            Console.WriteLine("exception");
        }

        // The first matching handler wins, and finally runs.
        try
        {
            try
            {
                Console.WriteLine(Read(a, 4));
            }
            catch (NullReferenceException e)
            {
                Console.WriteLine("wrong handler");
            }
            finally
            {
                Console.WriteLine("finally");
            }
        }
        catch (OutOfRangeException e)
        {
            Console.WriteLine("outer handler");
        }

        Console.WriteLine("done");
    }
}
