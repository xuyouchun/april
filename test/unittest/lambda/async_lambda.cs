// An async lambda: async () => ... is a lambda that makes a task (its body runs in a task, the awaits wait in the thread of it).

/* EXPECT

3
8
ok
12

*/

import System;

using System;
using System.Threading.Tasks;

class Program
{
    static Task<Int32> Twice(Int32 x)
    {
        return Task.Run(() => x * 2);
    }

    [EntryPoint]
    public static void Main()
    {
        Func<Task<Int32>> f = async () => 3;
        Console.WriteLine(f().GetAwaiter().GetResult());

        Func<Task<Int32>, Int32> g = async (x) => (await Twice(x)) + 0;
        Console.WriteLine(g(4).GetAwaiter().GetResult());

        Int32 flag = 0;
        Func<Task> h = async () => { await Twice(1); flag = 1; };
        h().GetAwaiter().GetResult();
        Console.WriteLine(flag == 1? "ok" : "bad");

        Func<Task<Int32>, Int32, Int32> k = async (a, b) => { Int32 s = await Twice(a); return s + b; };
        Console.WriteLine(k(5, 2).GetAwaiter().GetResult());
    }
}
