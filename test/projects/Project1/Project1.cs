
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        try
        {
            Console.WriteLine("-------- Try");

            throw new Exception();
        }
        catch(Exception ex)
        {
            Console.WriteLine("-------- Catch");
        }
        finally
        {
            Console.WriteLine("-------- finally");
        }
    }
};


