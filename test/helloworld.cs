import System;

using System;
using System.Collections;
using System.Linq;
using System.Net.Http;
using System.Threading.Tasks;

typedef MyArray<T> = T[];
typedef MyList<T> = List<MyArray<T>>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        try
        {
            MyList<int> list = new MyList<int>();
            list.Add(new MyArray<int> { 1, 2, 3 });

            Console.WriteLine($"Hello, World! - {list[0][1]}");
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.Message);
        }
    }
 }

