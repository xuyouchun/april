// A method that returns Task or Task<T> and has await is a state machine: it runs until an await whose task has not ended, and goes
// on in the thread pool when it ends. return gives the value of the task, an exception that it throws is the one of the task (await
// throws it as it is), and try/catch/finally work around the awaits.

/* EXPECT

started
value 42
loop 0 10 20 30
nested 7
sync 5
caught: boom
finally ran
after catch 1
catches 1
faulted True boom
canceled True
void done
args 13
foreach 6
instance 105

*/

import System;

using System;
using System.Threading;
using System.Threading.Tasks;

class Counter
{
    Int32 __base = 100;

    public Task<Int32> AddAsync(Int32 value)
    {
        Int32 x = await Program.SlowAsync(5);
        return __base + value + x - 5;
    }
}

class Program
{
    static Int32 Slow(Int32 value)
    {
        Thread.Sleep(20);
        return value;
    }

    public static Task<Int32> SlowAsync(Int32 value)
    {
        Delegate<Int32> work = () => Slow(value);
        return Task.Run(work);
    }

    static Task<Int32> Value()
    {
        Int32 a = await SlowAsync(40);
        return a + 2;
    }

    static Task<String> Loop()
    {
        String text = "";
        for (Int32 i = 0; i < 4; i++)
        {
            Int32 v = await SlowAsync(i * 10);
            text += (i == 0? "" : " ") + v;
        }

        return text;
    }

    static Task<Int32> Nested()
    {
        return await SlowAsync(await SlowAsync(3) + 1) + 3;
    }

    // Has no await: it is an ordinary method that returns a task.
    static Task<Int32> Plain()
    {
        return Task.FromResult(5);
    }

    static Task<Int32> Fails()
    {
        await SlowAsync(1);
        throw new InvalidOperationException("boom");
    }

    static Task<Int32> Catches()
    {
        Int32 r = 0;

        try
        {
            await SlowAsync(1);
            r = await Fails();
        }
        catch (InvalidOperationException e)
        {
            Console.WriteLine("caught: " + e.Message);
        }
        finally
        {
            Console.WriteLine("finally ran");
        }

        r = r + await SlowAsync(1);
        Console.WriteLine("after catch " + r);
        return r;
    }

    static Task Cancels(CancellationToken token)
    {
        await SlowAsync(1);
        token.ThrowIfCancellationRequested();
    }

    static Task Nothing()
    {
        await SlowAsync(1);
        Console.WriteLine("void done");
    }

    static Int32 Add(Int32 a, Int32 b)
    {
        return a + b;
    }

    static Task<Int32> InArgs()
    {
        return Add(await SlowAsync(6), await SlowAsync(7));
    }

    static Task<Int32> Each()
    {
        Int32 total = 0;
        Int32[] items = new Int32[] { 1, 2, 3 };

        foreach (Int32 item in items)
            total += await SlowAsync(item);

        return total;
    }

    [EntryPoint]
    public static void Main()
    {
        Task<Int32> value = Value();
        Console.WriteLine("started");
        Console.WriteLine("value " + value.Result);

        Console.WriteLine("loop " + Loop().Result);
        Console.WriteLine("nested " + Nested().Result);
        Console.WriteLine("sync " + Plain().Result);

        Console.WriteLine("catches " + Catches().Result);

        Task<Int32> fails = Fails();
        try { fails.Wait(); }
        catch (AggregateException e)
        {
            Console.WriteLine("faulted " + fails.IsFaulted + " " + e.InnerExceptions[0].Message);
        }

        CancellationTokenSource source = new CancellationTokenSource();
        source.Cancel();
        Task canceled = Cancels(source.Token);
        try { canceled.Wait(); }
        catch (AggregateException e) { }
        Console.WriteLine("canceled " + canceled.IsCanceled);

        Nothing().Wait();
        Console.WriteLine("args " + InArgs().Result);
        Console.WriteLine("foreach " + Each().Result);

        Counter counter = new Counter();
        Console.WriteLine("instance " + counter.AddAsync(5).Result);
    }
}
