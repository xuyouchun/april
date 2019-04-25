
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        try
        {
            Class1 obj1 = new Class1();
            Console.WriteLine("------------");

            throw new Exception("ABCDEFG");
        }
        catch(Exception ex)
        {
            Console.WriteLine("-------- catch");
        }
        finally
        {
            Console.WriteLine("-------- finally");
        }
    }
};

class Class1
{

};
