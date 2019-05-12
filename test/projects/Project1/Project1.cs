
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        try
        {
            for(int k = 0; k < 2; k++)
            {
                try
                {
                    try
                    {
                        Console.Print(k);
                        throw new Exception();
                    }
                    catch(Exception e)
                    {
                        Console.WriteLine("++++++++++++++++++++ Catch");
                    }
                    finally
                    {
                        Console.WriteLine("++++++++++++++++++++ Finally");
                        __Throw2();
                    }
                }
                finally
                {
                    Console.WriteLine("++++++++++++++++++++ Finally !");
                }
            }
        }
        catch(Exception e)
        {
            Console.WriteLine("++++++++++++++++++++ Catch !!");
        }
        finally
        {
            Console.WriteLine("++++++++++++++++++++ Finally !!");
        }
    }

    public static void __Throw2()
    {
        try
        {
            throw new Exception();
        }
        finally
        {
            Console.WriteLine("++++++++++++++++++++ __Throw2: Finally");
        }
    }

    public static void __DoSomething()
    {
        throw new Exception();
    }
};


