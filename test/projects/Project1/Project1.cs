
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
                try
                {
                    Console.WriteLine("---------- Try");
                    //throw new Exception();
                }
                finally
                {
                    throw new Exception();
                }
            }
            catch(Exception e)
            {
                Console.WriteLine("--------- Catch");
            }
        }
        catch(Exception e)
        {
            Console.WriteLine("--------- Catch !");
        }
    }

    public static void __DoSomething()
    {
        throw new Exception();
    }
};


