
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        try
        {
            int a = 1, b = 2;
            Console.Print(a + b);

            Console.WriteLine("Try");
            throw new Exception("Hello Exception!");
        }
        catch(Exception ex)
        {
            Console.WriteLine("Catch");
        }
        finally
        {
            Console.WriteLine("Finally");
        }
    }
};

