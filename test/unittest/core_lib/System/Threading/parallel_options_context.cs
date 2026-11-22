// Parallel loops with a state and options, the options of ContinueWith, and AsyncLocal<T>: the value goes with the flow of the work
// (a thread, a task, a timer, the code after an await, a continuation) and a change is seen only by what is started after it.

/* EXPECT

for 100 True
break False 5
break below True
stop False
state exit True
degree 2
canceled OperationCanceledException
foreach state 3 False
fault OnlyOnFaulted ran | NotOnFaulted canceled True True
ok OnlyOnRanToCompletion ran 7 | NotOnRanToCompletion canceled True
token canceled True
sync True
local main 1
thread 1
task 1
pool 1
change stays 2 1
await 1 then 3
continuation 1
timer 1
default 0 7
done

*/

import System;

using System;
using System.Collections;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    static AsyncLocal<Int32> __local = new AsyncLocal<Int32>();
    static Int32 __threadValue;
    static Int32 __timerValue;
    static Int32 __poolValue;
    static Int32 __active;
    static Int32 __maxActive;
    static Int32 __count;
    static Boolean __exit;
    static String __log = "";
    static Object __gate = new Object();

    static Void Worker(Object o)
    {
        __threadValue = __local.Value;
    }

    static Void Pool(Object o)
    {
        __poolValue = __local.Value;
    }

    static Void Tick(Object o)
    {
        __timerValue = __local.Value;
    }

    static Int32 ReadLocal()
    {
        return __local.Value;
    }

    static Int32 ChangeLocal()
    {
        __local.Value = 2;
        return __local.Value;
    }

    static Int32 Fail()
    {
        throw new InvalidOperationException("x");
    }

    static Task<Int32> Async()
    {
        __local.Value = 1;
        await Task.Delay(20);
        Int32 first = __local.Value;

        __local.Value = 3;
        await Task.Yield();
        Console.WriteLine("await " + first + " then " + __local.Value);

        return __local.Value;
    }

    static Void Slow(Int32 i)
    {
        Int32 now = Interlocked.Increment(ref __active);
        lock (__gate)
        {
            if (now > __maxActive)
                __maxActive = now;
        }

        Thread.Sleep(5);
        Interlocked.Decrement(ref __active);
    }

    static Void CountAndStop(Int32 i, ParallelLoopState state)
    {
        Interlocked.Increment(ref __count);
        state.Stop();
    }

    static Void ExitCheck(Int32 i, ParallelLoopState state)
    {
        if (i == 0)
        {
            state.Stop();
            __exit = state.ShouldExitCurrentIteration;
        }
    }

    static Void FaultedRan(Task t)
    {
        __log += "OnlyOnFaulted ran";
    }

    static Void Never(Task t)
    {
        __log += "never";
    }

    static Void Nothing(Task t)
    {
    }

    static Int32 ResultOf(Task<Int32> t)
    {
        return t.Result;
    }

    [EntryPoint]
    public static void Main()
    {
        // The loops.
        Int32[] hits = new Int32[100];
        Delegate<Void, Int32> mark = i => { hits[i] = 1; };
        ParallelLoopResult all = Parallel.For(0, 100, mark);
        Int32 sum = 0;
        for (Int32 i = 0; i < 100; i++)
            sum += hits[i];

        Console.WriteLine("for " + sum + " " + all.IsCompleted);

        // Break: the iterations before the one that breaks run, the ones after do not begin.
        Int32[] ran = new Int32[200];
        Delegate<Void, Int32, ParallelLoopState> breaking = (i, state) => {
            ran[i] = 1;
            if (i == 5)
                state.Break();
        };

        ParallelLoopResult broken = Parallel.For(0, 200, breaking);

        Boolean below = true;
        for (Int32 i = 0; i <= 5; i++)
        {
            if (ran[i] == 0)
                below = false;
        }

        Console.WriteLine("break " + broken.IsCompleted + " " + broken.LowestBreakIteration);
        Console.WriteLine("break below " + below);

        Delegate<Void, Int32, ParallelLoopState> stopping = CountAndStop;
        ParallelLoopResult stopped = Parallel.For(0, 100000, stopping);
        Console.WriteLine("stop " + stopped.IsCompleted);

        Delegate<Void, Int32, ParallelLoopState> check = ExitCheck;
        Parallel.For(0, 4, check);
        Console.WriteLine("state exit " + __exit);

        // The degree.
        ParallelOptions options = new ParallelOptions();
        options.MaxDegreeOfParallelism = 2;

        Delegate<Void, Int32> slow = Slow;
        Parallel.For(0, 20, options, slow);
        Console.WriteLine("degree " + __maxActive);

        // The token.
        CancellationTokenSource source = new CancellationTokenSource();
        source.Cancel();
        ParallelOptions canceledOptions = new ParallelOptions();
        canceledOptions.CancellationToken = source.Token;

        Delegate<Void, Int32> nothing = i => { };
        try { Parallel.For(0, 10, canceledOptions, nothing); }
        catch (OperationCanceledException e) { Console.WriteLine("canceled OperationCanceledException"); }

        List<Int32> items = new List<Int32>();
        for (Int32 i = 0; i < 10; i++)
            items.Add(i);

        Delegate<Void, Int32, ParallelLoopState> breakAtTwo = (item, state) => {
            if (item == 2)
                state.Break();
        };

        ParallelLoopResult each = Parallel.ForEach(items, breakAtTwo);
        Console.WriteLine("foreach state " + (each.LowestBreakIteration + 1) + " " + each.IsCompleted);

        // The options of ContinueWith.
        Delegate<Int32> fail = Fail;
        Task<Int32> bad = Task.Run(fail);
        try { bad.Wait(); } catch (AggregateException e) { }

        Delegate<Void, Task<Int32>> faultedRan = t => { __log += "OnlyOnFaulted ran"; };
        Delegate<Void, Task<Int32>> never = t => { __log += "never"; };

        Task a = bad.ContinueWith(faultedRan, TaskContinuationOptions.OnlyOnFaulted);
        Task b = bad.ContinueWith(never, TaskContinuationOptions.NotOnFaulted);
        a.Wait();
        try { b.Wait(); } catch (AggregateException e) { }

        Console.WriteLine("fault " + __log + " | NotOnFaulted canceled " + b.IsCanceled + " " + !__log.Contains("never"));

        Task<Int32> good = Task.FromResult(7);
        Delegate<Int32, Task<Int32>> resultOf = ResultOf;
        Delegate<Void, Task<Int32>> doNothing = t => { };

        Task<Int32> c = good.ContinueWith<Int32>(resultOf, TaskContinuationOptions.OnlyOnRanToCompletion);
        Task d = good.ContinueWith(doNothing, TaskContinuationOptions.NotOnRanToCompletion);
        try { d.Wait(); } catch (AggregateException e) { }

        Console.WriteLine("ok OnlyOnRanToCompletion ran " + c.Result + " | NotOnRanToCompletion canceled " + d.IsCanceled);

        CancellationTokenSource tokenSource = new CancellationTokenSource();
        tokenSource.Cancel();
        Task e1 = good.ContinueWith(doNothing, tokenSource.Token);
        try { e1.Wait(); } catch (AggregateException e) { }
        Console.WriteLine("token canceled " + e1.IsCanceled);

        Task f = good.ContinueWith(doNothing, TaskContinuationOptions.ExecuteSynchronously);
        f.Wait();
        Console.WriteLine("sync " + f.IsCompletedSuccessfully);

        // AsyncLocal.
        __local.Value = 1;
        Console.WriteLine("local main " + __local.Value);

        Thread worker = new Thread(Worker);
        worker.Start(null);
        worker.Join();
        Console.WriteLine("thread " + __threadValue);

        Delegate<Int32> read = ReadLocal;
        Console.WriteLine("task " + Task.Run(read).Result);

        Delegate<Void, Object> pool = Pool;
        ThreadPool.QueueUserWorkItem(pool);
        while (__poolValue == 0)
            Thread.Sleep(5);

        Console.WriteLine("pool " + __poolValue);

        // A change inside a task is not seen by the code that started it.
        Delegate<Int32> change = ChangeLocal;
        Int32 inner = Task.Run(change).Result;
        Console.WriteLine("change stays " + inner + " " + __local.Value);

        // The code after an await: it has the values of the method.
        __local.Value = 0;
        Int32 result = Async().Result;
        Console.WriteLine("continuation " + (result == 3? 1 : 0));

        __local.Value = 1;
        Delegate<Void, Object> tick = Tick;
        Timer timer = new Timer(tick, null, 10, -1);
        Thread.Sleep(150);
        Console.WriteLine("timer " + __timerValue);

        AsyncLocal<Int32> other = new AsyncLocal<Int32>();
        other.Value = 7;
        Console.WriteLine("default " + new AsyncLocal<Int32>().Value + " " + other.Value);
        Console.WriteLine("done");
    }
}
