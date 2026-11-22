// IAsyncEnumerable<T>: a method that returns it and has yield return (and await) makes the items one by one, each MoveNextAsync runs
// it until the next one; await foreach goes through it. SemaphoreSlim.LockAsync gives mutual exclusion that await can hold.

/* EXPECT

items 1 2 3 4
break 10 20
async sum 60
await foreach in a task 6
locked 3
yield 3

*/

import System;

using System;
using System.Collections;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    static Task<Int32> Later(Int32 value)
    {
        Delegate<Int32> work = () => { Thread.Sleep(10); return value; };
        return Task.Run(work);
    }

    // Waits for each item.
    static IAsyncEnumerable<Int32> Numbers(Int32 count)
    {
        for (Int32 i = 1; i <= count; i++)
        {
            Int32 v = await Later(i);
            yield return v;
        }
    }

    static IAsyncEnumerable<Int32> Tens()
    {
        yield return 10;
        yield return 20;
        yield return 30;
        yield break;
    }

    static Task<Int32> Sum()
    {
        Int32 total = 0;
        await foreach (Int32 n in Tens())
            total += await Later(n);

        return total;
    }

    static Task<Int32> InTask()
    {
        Int32 total = 0;
        await foreach (Int32 n in Numbers(3))
            total += n;

        return total;
    }

    static Task<Int32> Locked(SemaphoreSlim gate, Int32[] counter)
    {
        using (await gate.LockAsync())
        {
            Int32 seen = counter[0];
            await Task.Yield();
            counter[0] = seen + 1;
        }

        return counter[0];
    }

    [EntryPoint]
    public static void Main()
    {
        String text = "";
        await foreach (Int32 n in Numbers(4))
            text += (text == ""? "" : " ") + n;

        Console.WriteLine("items " + text);

        String early = "";
        await foreach (Int32 n in Tens())
        {
            if (n > 20)
                break;

            early += (early == ""? "" : " ") + n;
        }

        Console.WriteLine("break " + early);
        Console.WriteLine("async sum " + Sum().Result);
        Console.WriteLine("await foreach in a task " + InTask().Result);

        SemaphoreSlim gate = new SemaphoreSlim(1);
        Int32[] counter = new Int32[1];
        Task<Int32>[] tasks = new Task<Int32>[3];

        for (Int32 i = 0; i < 3; i++)
            tasks[i] = Locked(gate, counter);

        for (Int32 i = 0; i < 3; i++)
            tasks[i].Wait();

        Console.WriteLine("locked " + counter[0]);

        Task yielded = Task.Yield();
        yielded.Wait();
        Console.WriteLine("yield " + 3);
    }
}
