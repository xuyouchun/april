/*
 *  System.dll
 *
 *  Parallel: loops whose iterations run in the thread pool.
 */

using Collections;

namespace Threading.Tasks;

////////// ////////// ////////// ////////// //////////

// What a parallel loop is told: the most threads that work at the same time (-1: one for each processor) and the token that
// stops it (the loop throws OperationCanceledException when it is canceled).
public class ParallelOptions
{
    private Int32 __degree = -1;
    private CancellationToken __token;

    public Int32 MaxDegreeOfParallelism
    {
        get { return __degree; }
        set
        {
            if (value == 0 || value < -1)
                throw new ArgumentException("the degree of parallelism is -1 or more than 0");

            __degree = value;
        }
    }

    public CancellationToken CancellationToken
    {
        get { return __token; }
        set { __token = value; }
    }
}

// How a parallel loop ended.
public class ParallelLoopResult
{
    private Boolean __completed;
    private Int64 __lowestBreak;

    internal ParallelLoopResult(Boolean completed, Int64 lowestBreak)
    {
        __completed = completed;
        __lowestBreak = lowestBreak;
    }

    // Whether all the iterations ran (no one called Break or Stop).
    public Boolean IsCompleted => __completed;

    // The least index of the iterations that called Break (-1 when there was none): all the iterations before it ran.
    public Int64 LowestBreakIteration => __lowestBreak;

    public Boolean HasLowestBreakIteration => __lowestBreak >= 0;
}

// What an iteration can do to the loop that it is in: stop it (Stop), or stop the iterations that are after it (Break).
public class ParallelLoopState
{
    private ParallelRange __range;
    private Int64 __index;

    internal ParallelLoopState(ParallelRange range)
    {
        __range = range;
    }

    internal Void Begin(Int64 index)
    {
        __index = index;
    }

    // Whether Stop or Break were called, or an iteration threw: the iteration that runs can end early.
    public Boolean ShouldExitCurrentIteration => __range.ShouldExit(__index);

    public Boolean IsStopped => __range.Stopped;

    public Boolean IsExceptional => __range.Failed;

    // The least index of the iterations that called Break (-1 when there was none).
    public Int64 LowestBreakIteration => __range.LowestBreak();

    // No iteration begins after this call (the ones that began end).
    public Void Stop()
    {
        __range.Stop();
    }

    // No iteration that is after this one begins (the ones before it still run).
    public Void Break()
    {
        __range.Break(__index);
    }
}

// A part of a loop: the iterations from `Next` to `End`, taken by the workers one at a time.
internal sealed class ParallelRange
{
    internal Int32 Next;
    internal Int32 End;
    internal Delegate<Void, Int32> Body;
    internal Delegate<Void, Int32, ParallelLoopState> StateBody;
    internal Boolean HasState;
    internal CancellationToken Token;
    internal List<Exception> Errors = new List<Exception>();
    internal Boolean Failed;
    internal Boolean Stopped;
    private Int64 __break = -1;

    internal Boolean Canceled => Token.IsCancellationRequested;

    internal Void Stop()
    {
        lock (this)
            Stopped = true;
    }

    internal Void Break(Int64 index)
    {
        lock (this)
        {
            if (__break < 0 || index < __break)
                __break = index;
        }
    }

    internal Int64 LowestBreak()
    {
        lock (this)
            return __break;
    }

    internal Boolean ShouldExit(Int64 index)
    {
        lock (this)
            return Stopped || Failed || (__break >= 0 && __break < index) || Token.IsCancellationRequested;
    }

    // Takes the next iteration, -1 when there is none: the loop was stopped, an iteration failed or called Break (the ones after it
    // do not begin), or the token was canceled.
    internal Int32 Take()
    {
        lock (this)
        {
            if (Failed || Stopped || Next >= End || Token.IsCancellationRequested)
                return -1;

            if (__break >= 0 && Next > __break)
                return -1;

            Int32 index = Next;
            Next = Next + 1;

            return index;
        }
    }

    // A worker: runs iterations until there is none.
    internal Void Work()
    {
        ParallelLoopState state = new ParallelLoopState(this);

        for (;;)
        {
            Int32 index = Take();
            if (index < 0)
                return;

            try
            {
                if (HasState)
                {
                    state.Begin(index);
                    StateBody(index, state);
                }
                else
                {
                    Body(index);
                }
            }
            catch (Exception e)
            {
                lock (this)
                {
                    Failed = true;
                    Errors.Add(e);
                }

                return;
            }
        }
    }
}

// Loops and calls that run in parallel; they end when all the work has. An exception of an iteration stops the loop (the
// iterations that began end) and is thrown, in an AggregateException with the others.
public static class Parallel
{
    // Runs body(index) for the indexes from `fromInclusive` to `toExclusive`.
    public static ParallelLoopResult For(Int32 fromInclusive, Int32 toExclusive, Delegate<Void, Int32> body)
    {
        return For(fromInclusive, toExclusive, new ParallelOptions(), body);
    }

    public static ParallelLoopResult For(Int32 fromInclusive, Int32 toExclusive, ParallelOptions options, Delegate<Void, Int32> body)
    {
        ParallelRange range = new ParallelRange();
        range.Body = body;

        return Run(range, fromInclusive, toExclusive, options);
    }

    // The body has the state of the loop: state.Break(), state.Stop().
    public static ParallelLoopResult For(Int32 fromInclusive, Int32 toExclusive, Delegate<Void, Int32, ParallelLoopState> body)
    {
        return For(fromInclusive, toExclusive, new ParallelOptions(), body);
    }

    public static ParallelLoopResult For(Int32 fromInclusive, Int32 toExclusive, ParallelOptions options,
                                         Delegate<Void, Int32, ParallelLoopState> body)
    {
        ParallelRange range = new ParallelRange();
        range.StateBody = body;
        range.HasState = true;

        return Run(range, fromInclusive, toExclusive, options);
    }

    private static ParallelLoopResult Run(ParallelRange range, Int32 fromInclusive, Int32 toExclusive, ParallelOptions options)
    {
        if (options.CancellationToken.IsCancellationRequested)
            throw new OperationCanceledException();

        if (toExclusive <= fromInclusive)
            return new ParallelLoopResult(true, -1);

        range.Next = fromInclusive;
        range.End = toExclusive;
        range.Token = options.CancellationToken;

        // The thread that calls is a worker too: the others are tasks of the pool.
        Int32 count = toExclusive - fromInclusive;
        Int32 degree = options.MaxDegreeOfParallelism > 0? options.MaxDegreeOfParallelism : Environment.ProcessorCount;
        Int32 helpers = (count < degree? count : degree) - 1;

        Delegate<Void> work = range.Work;
        Task[] tasks = new Task[helpers];

        for (Int32 k = 0; k < helpers; k++)
            tasks[k] = Task.Run(work);

        range.Work();

        for (Int32 k = 0; k < helpers; k++)
            tasks[k].Wait();

        if (range.Errors.Count > 0)
            throw new AggregateException(range.Errors.ToArray());

        if (range.Canceled)
            throw new OperationCanceledException();

        Int64 lowest = range.LowestBreak();
        return new ParallelLoopResult(!range.Stopped && lowest < 0, lowest);
    }

    // Runs body(item) for each item.
    public static ParallelLoopResult ForEach<T>(IEnumerable<T> source, Delegate<Void, T> body)
    {
        return ForEach(source, new ParallelOptions(), body);
    }

    public static ParallelLoopResult ForEach<T>(IEnumerable<T> source, ParallelOptions options, Delegate<Void, T> body)
    {
        ForEachState<T> state = new ForEachState<T>();
        state.Items = Copy(source);
        state.Body = body;

        Delegate<Void, Int32> run = state.Run;
        return For(0, state.Items.Count, options, run);
    }

    public static ParallelLoopResult ForEach<T>(IEnumerable<T> source, Delegate<Void, T, ParallelLoopState> body)
    {
        return ForEach(source, new ParallelOptions(), body);
    }

    public static ParallelLoopResult ForEach<T>(IEnumerable<T> source, ParallelOptions options, Delegate<Void, T, ParallelLoopState> body)
    {
        ForEachState<T> state = new ForEachState<T>();
        state.Items = Copy(source);
        state.StateBody = body;

        Delegate<Void, Int32, ParallelLoopState> run = state.RunWithState;
        return For(0, state.Items.Count, options, run);
    }

    private static List<T> Copy<T>(IEnumerable<T> source)
    {
        List<T> items = new List<T>();
        IEnumerator<T> enumerator = source.GetEnumerator();

        while (enumerator.MoveNext())
            items.Add(enumerator.Current);

        return items;
    }

    // Runs the methods, and waits for all of them.
    public static Void Invoke(Delegate<Void>[] actions)
    {
        Invoke(new ParallelOptions(), actions);
    }

    public static Void Invoke(ParallelOptions options, Delegate<Void>[] actions)
    {
        InvokeState state = new InvokeState();
        state.Actions = actions;

        Delegate<Void, Int32> run = state.Run;
        For(0, actions.Length, options, run);
    }
}

internal sealed class ForEachState<T>
{
    internal List<T> Items;
    internal Delegate<Void, T> Body;
    internal Delegate<Void, T, ParallelLoopState> StateBody;

    internal Void Run(Int32 index)
    {
        Body(Items[index]);
    }

    internal Void RunWithState(Int32 index, ParallelLoopState state)
    {
        StateBody(Items[index], state);
    }
}

internal sealed class InvokeState
{
    internal Delegate<Void>[] Actions;

    internal Void Run(Int32 index)
    {
        Delegate<Void> action = Actions[index];
        action();
    }
}
