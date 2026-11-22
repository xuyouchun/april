// Task and Task<T>: Run, Result, Status, an exception (AggregateException), the cancellation, ContinueWith, WhenAll, WhenAny,
// WaitAll, Delay, FromResult, CompletedTask and TaskCompletionSource<T>.

/* EXPECT

run 42 RanToCompletion True
fault Faulted True boom 1
cancel Canceled True True
token Canceled
continue 43 done
whenall 3 6 RanToCompletion
whenall fault 2
whenany True
waitall True True
delay RanToCompletion True
completed True 7 True
source 99 True Canceled Faulted

*/

import System;

using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    static Int32 Compute() { return 42; }

    static Void Fail() { throw new InvalidOperationException("boom"); }

    static Void Cancel() { throw new OperationCanceledException("stop"); }

    static Void Nothing() { }

    static Int32 Slow()
    {
        Thread.Sleep(30);
        return 5;
    }

    static Int32 Plus(Task<Int32> previous) { return previous.Result + 1; }

    static Void After(Task<Int32> previous) { }

    static Int32 Fast() { return 1; }

    static Int32 Two() { return 2; }

    static Int32 Three() { return 3; }

    static Void Fail1() { throw new InvalidOperationException("one"); }

    static Void Fail2() { throw new ArgumentException("two"); }

    [EntryPoint]
    public static void Main()
    {
        Task<Int32> run = Task.Run<Int32>(Compute);
        Console.WriteLine("run " + run.Result + " " + run.Status + " " + run.IsCompletedSuccessfully);

        Task fault = Task.Run(Fail);
        String message = "";
        try { fault.Wait(); } catch (AggregateException e) { message = e.InnerExceptions[0].Message + " " + e.InnerExceptions.Length; }
        Console.WriteLine("fault " + fault.Status + " " + fault.IsFaulted + " " + message);

        Task canceled = Task.Run(Cancel);
        Boolean caught = false;
        try { canceled.Wait(); } catch (AggregateException e) { caught = true; }
        Console.WriteLine("cancel " + canceled.Status + " " + canceled.IsCanceled + " " + caught);

        CancellationTokenSource source = new CancellationTokenSource();
        source.Cancel();
        Task never = Task.Run(Nothing, source.Token);
        try { never.Wait(); } catch (AggregateException e) { }
        Console.WriteLine("token " + never.Status);

        Delegate<Int32, Task<Int32>> plus = Plus;
        Delegate<Void, Task<Int32>> after = After;
        Task<Int32> next = run.ContinueWith<Int32>(plus);
        Task last = next.ContinueWith(after);
        last.Wait();
        Console.WriteLine("continue " + next.Result + " done");

        Task<Int32>[] results = new Task<Int32>[3];
        results[0] = Task.Run<Int32>(Fast);
        results[1] = Task.Run<Int32>(Two);
        results[2] = Task.Run<Int32>(Three);
        Task<Array<Int32>> all = Task.WhenAll<Int32>(results);
        Array<Int32> values = all.Result;
        Console.WriteLine("whenall " + values.Length + " " + (values[0] + values[1] + values[2]) + " " + all.Status);

        Task[] faulty = new Task[3];
        faulty[0] = Task.Run(Fail1);
        faulty[1] = Task.Run(Fail2);
        faulty[2] = Task.Run(Nothing);
        Task allFaulty = Task.WhenAll(faulty);
        Int32 errors = 0;
        try { allFaulty.Wait(); } catch (AggregateException e) { errors = e.InnerExceptions.Length; }
        Console.WriteLine("whenall fault " + errors);

        Task<Int32> slow = Task.Run<Int32>(Slow);
        Task<Int32> fast = Task.Run<Int32>(Fast);
        Task[] race = new Task[2];
        race[0] = slow;
        race[1] = fast;
        Task winner = Task.WhenAny(race).Result;
        Console.WriteLine("whenany " + Object.ReferenceEquals(winner, fast));

        Task[] two = new Task[2];
        two[0] = Task.Run(Nothing);
        two[1] = Task.Run<Int32>(Slow);
        Task.WaitAll(two);
        Console.WriteLine("waitall " + two[0].IsCompleted + " " + two[1].IsCompleted);

        Int64 start = Environment.TickCount64;
        Task delay = Task.Delay(60);
        delay.Wait();
        Console.WriteLine("delay " + delay.Status + " " + (Environment.TickCount64 - start >= 50));

        Task<Int32> fromResult = Task.FromResult<Int32>(7);
        Console.WriteLine("completed " + Task.CompletedTask.IsCompleted + " " + fromResult.Result + " " + fromResult.IsCompleted);

        TaskCompletionSource<Int32> a = new TaskCompletionSource<Int32>(), b = new TaskCompletionSource<Int32>(), c = new TaskCompletionSource<Int32>();
        Task<Int32> waiting = a.Task;
        Task.Run(Nothing).Wait();
        a.SetResult(99);
        b.SetCanceled();
        c.SetException(new InvalidOperationException("x"));
        Console.WriteLine("source " + waiting.Result + " " + !a.TrySetResult(1) + " " + b.Task.Status + " " + c.Task.Status);
    }
}
