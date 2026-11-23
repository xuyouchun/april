import System;

using System;
using System.Collections;
using System.Linq;
using System.Net.Http;
using System.Threading.Tasks;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        try
        {
            typedef Dict = System.Collections.Dictionary<int, int>;

            Dict dict = new Dict();
            dict[1] = 111;
            dict[2] = 222;

            Console.WriteLine($"{dict[1]}, {dict[2]}");
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.Message);
        }
    }

    private static Task<int> __GetValueAsync()
    {
        return Task.Run(() => {
            return 42;
        });
    }

 }

