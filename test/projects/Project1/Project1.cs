
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        try
        {
            try
            {
                int a = 1, b = 2;
                Console.Print(a + b);
            }
            catch(Exception ex)
            {
                int a = 1, b = 2;
                Console.Print(a + b);
            }
        }
        catch(Exception ex)
        {
            Console.WriteLine("Catch");
        }
        finally
        {
            try
            {
                int a = 1, b = 2;
                Console.Print(a + b);
            }
            catch(Exception ex)
            {
                Console.WriteLine("haha");
            }

            Console.WriteLine("Finally");
        }
    }
};

