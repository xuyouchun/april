// Task.Run(lambda) is the generic Run<T> when the lambda returns a value: T is the type of the value.

/* EXPECT

block 42
expression 7
literal text
void ran
done

*/

import System;

using System;
using System.Threading.Tasks;

class Program
{
    static Boolean __ran;

    static Task<Int32> Block()
    {
        return Task.Run(() => {
            return 42;
        });
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("block " + Block().Result);

        Int32 x = 3;
        Task<Int32> e = Task.Run(() => x + 4);
        Console.WriteLine("expression " + e.Result);

        Task<String> s = Task.Run(() => "text");
        Console.WriteLine("literal " + s.Result);

        Task v = Task.Run(() => { __ran = true; });
        v.Wait();
        Console.WriteLine("void " + (__ran ? "ran" : "no"));
        Console.WriteLine("done");
    }
}
