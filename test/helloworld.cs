import System;

using System;
using System.Collections;
using System.Linq;
using System.Net.Http;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        try
        {
            string s = "world";
            // string sss = $"Hello {s}!";

            string sss = $"""
            Hi, \n
            Hello {s}!!!
            """;

            Console.WriteLine(sss);
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.Message);
        }
    }

    private static void PrintArray(IEnumerable<int> arr)
    {
        foreach (int i in arr)
        {
            Console.WriteLine(i);
        }
    }
}

