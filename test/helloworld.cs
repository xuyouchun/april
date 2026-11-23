import System;

using System;
using System.Collections;
using System.Linq;
using System.Net.Http;
using System.Threading.Tasks;

typedef MyTuple<T1, T2> = Tuple<Tuple<T1, T2>, Tuple<T1, T2>, Tuple<T1, T2>>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        try
        {
            var tuple = (Name: "Wangbo", Value: 2, 3);
            var tuple2 = tuple;

            Console.WriteLine($"{tuple2.Name}");
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.Message);
        }
    }
 }

