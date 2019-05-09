
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
                    throw new Exception();
                }
                finally
                {
                    Console.WriteLine("---------- Finally");
                    // throw new Exception();
                    __GetValue();
                }
            }
            finally
            {
                Console.WriteLine("--------- Finally !");
            }
        }
        catch(Exception e)
        {
            Console.WriteLine("--------- Catch !!");
        }
    }

    public static int __GetValue()
    {
        throw new Exception();
    }

    public static void __DoSomething()
    {
        throw new Exception();
    }
};


