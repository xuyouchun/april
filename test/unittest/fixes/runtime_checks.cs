// Invalid operations raise exceptions instead of crashing the process.

/* EXPECT

null field read
null field write
null call
null array length
null array read
null array write
throw null
divide by zero
modulo by zero
overflow
negative index
index too large
negative size
5
2
done

*/

import System;

using System;

class Node
{
    public int F;
    public int Get() { return F; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int zero = 0, minus = -1, min = -2147483647 - 1;
        Node n = null;
        int[] none = null;
        int[] arr = new int[2];

        try { int x = n.F; Console.WriteLine(x); }
        catch (NullReferenceException) { Console.WriteLine("null field read"); }

        try { n.F = 1; }
        catch (NullReferenceException) { Console.WriteLine("null field write"); }

        try { Console.WriteLine(n.Get()); }
        catch (NullReferenceException) { Console.WriteLine("null call"); }

        try { Console.WriteLine(none.Length); }
        catch (NullReferenceException) { Console.WriteLine("null array length"); }

        try { Console.WriteLine(none[0]); }
        catch (NullReferenceException) { Console.WriteLine("null array read"); }

        try { none[0] = 1; }
        catch (NullReferenceException) { Console.WriteLine("null array write"); }

        try { throw null; }
        catch (NullReferenceException) { Console.WriteLine("throw null"); }

        try { Console.WriteLine(5 / zero); }
        catch (DivideByZeroException) { Console.WriteLine("divide by zero"); }

        try { Console.WriteLine(5 % zero); }
        catch (DivideByZeroException) { Console.WriteLine("modulo by zero"); }

        try { Console.WriteLine(min / minus); }
        catch (OverflowException) { Console.WriteLine("overflow"); }

        try { Console.WriteLine(arr[minus]); }
        catch (OutOfRangeException) { Console.WriteLine("negative index"); }

        try { Console.WriteLine(arr[2]); }
        catch (OutOfRangeException) { Console.WriteLine("index too large"); }

        try { int[] bad = new int[minus]; Console.WriteLine(bad.Length); }
        catch (OutOfRangeException) { Console.WriteLine("negative size"); }

        // Valid operations still work.
        Console.WriteLine(10 / 2);
        Console.WriteLine(7 % 5);
        Console.WriteLine("done");
    }
}
