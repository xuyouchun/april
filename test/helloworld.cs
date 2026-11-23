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
            int value = await __GetValueAsync();

            Console.WriteLine(value);
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

