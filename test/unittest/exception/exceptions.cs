// try / catch / finally: the result of a return is computed before the finally block runs,
// throw; throws again, the catch blocks are part of the flow analysis.

/* EXPECT

f
40
5
c1
in
fin
1
a
b
done

*/

import System;

using System;

class Program
{
    static int ReturnInTry()
    {
        try
        {
            return 40;
        }
        finally
        {
            Console.WriteLine("f");
        }
    }

    // The method returns in the catch block only: no return is needed after the try.
    static int ReturnInCatch()
    {
        try
        {
            throw new Exception("x");
        }
        catch (Exception e)
        {
            return 5;
        }
    }

    // The value is computed before the finally block changes the variable.
    static int ValueBeforeFinally()
    {
        int x = 1;

        try
        {
            return x;
        }
        finally
        {
            x = 2;
            Console.WriteLine("fin");
        }
    }

    // The catch type is not Exception: the exception may leave the method, the end of the
    // method is still reachable.
    static void SpecificCatch()
    {
        try
        {
            Console.WriteLine("a");
        }
        catch (OutOfRangeException e)
        {
            Console.WriteLine("never");
        }

        Console.WriteLine("b");
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(ReturnInTry());
        Console.WriteLine(ReturnInCatch());

        try
        {
            try
            {
                throw new Exception("in");
            }
            catch (Exception e)
            {
                Console.WriteLine("c1");
                throw;
            }
        }
        catch (Exception e2)
        {
            Console.WriteLine(e2.Message);
        }

        Console.WriteLine(ValueBeforeFinally());

        SpecificCatch();
        Console.WriteLine("done");
    }
}
