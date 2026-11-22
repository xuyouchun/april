// `await` in a method that does not return a task waits in the thread: await task is task.GetAwaiter().GetResult(). The
// exception of a task is thrown as it is (not an AggregateException), the value of Task<T> is the result.

/* EXPECT

sum 42
void ran
inner caught: boom
canceled True
chain 15
plain 7

*/

import System;

using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    static Int32 Compute()
    {
        return 40;
    }

    static Void Work()
    {
        Thread.Sleep(10);
        Console.WriteLine("void ran");
    }

    static Int32 Fail()
    {
        throw new InvalidOperationException("boom");
    }

    static Int32 Add(Int32 a, Int32 b)
    {
        return a + b;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32 sum = await Task.Run(Compute) + 2;
        Console.WriteLine("sum " + sum);

        await Task.Run(Work);

        try
        {
            Int32 x = await Task.Run(Fail);
            Console.WriteLine("not here " + x);
        }
        catch (InvalidOperationException e)
        {
            Console.WriteLine("inner caught: " + e.Message);
        }

        CancellationTokenSource source = new CancellationTokenSource();
        source.Cancel();

        try
        {
            await Task.Run(Work, source.Token);
        }
        catch (OperationCanceledException e)
        {
            Console.WriteLine("canceled " + true);
        }

        Int32 chain = Add(await Task.Run(Compute) - 38, await Task.FromResult(13));
        Console.WriteLine("chain " + chain);

        Task<Int32> done = Task.FromResult(7);
        Console.WriteLine("plain " + await done);
    }
}
