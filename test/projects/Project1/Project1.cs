
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
            __DoSomething();
        }
        catch(Exception ex)
        {
            Console.WriteLine("-------- Catch");
        }
        finally
        {
            Console.WriteLine("-------- Finally");
        }
    }

    public static void __DoSomething()
    {
        try
        {
            //throw new Exception();
            Console.WriteLine("__DoSomething");
        }
        finally
        {
            Console.WriteLine("------- Finally !");
        }
    }
};


