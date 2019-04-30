
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        try
        {
            throw new Exception("Hello Exception");
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

