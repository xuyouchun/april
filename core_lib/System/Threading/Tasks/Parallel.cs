/*
 *  System.dll
 *
 *  Parallel: loops whose iterations run in the thread pool.
 */

using Collections;

namespace Threading.Tasks;

////////// ////////// ////////// ////////// //////////

// A part of a loop: the iterations from `Next` to `End`, taken by the workers one at a time.
internal sealed class ParallelRange
{
    internal Int32 Next;
    internal Int32 End;
    internal Delegate<Void, Int32> Body;
    internal List<Exception> Errors = new List<Exception>();
    internal Boolean Failed;

    // Takes the next iteration, -1 when there is none (or when an iteration failed: the loop stops).
    internal Int32 Take()
    {
        lock (this)
        {
            if (Failed || Next >= End)
                return -1;

            Int32 index = Next;
            Next = Next + 1;

            return index;
        }
    }

    // A worker: runs iterations until there is none.
    internal Void Work()
    {
        for (;;)
        {
            Int32 index = Take();
            if (index < 0)
                return;

            try
            {
                Body(index);
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
    public static Void For(Int32 fromInclusive, Int32 toExclusive, Delegate<Void, Int32> body)
    {
        if (toExclusive <= fromInclusive)
            return;

        ParallelRange range = new ParallelRange();
        range.Next = fromInclusive;
        range.End = toExclusive;
        range.Body = body;

        // The thread that calls is a worker too: the others are tasks of the pool.
        Int32 count = toExclusive - fromInclusive;
        Int32 helpers = (count < Environment.ProcessorCount? count : Environment.ProcessorCount) - 1;

        Delegate<Void> work = range.Work;
        Task[] tasks = new Task[helpers];

        for (Int32 k = 0; k < helpers; k++)
            tasks[k] = Task.Run(work);

        range.Work();

        for (Int32 k = 0; k < helpers; k++)
            tasks[k].Wait();

        if (range.Errors.Count > 0)
            throw new AggregateException(range.Errors.ToArray());
    }

    // Runs body(item) for each item.
    public static Void ForEach<T>(IEnumerable<T> source, Delegate<Void, T> body)
    {
        List<T> items = new List<T>();
        IEnumerator<T> enumerator = source.GetEnumerator();

        while (enumerator.MoveNext())
            items.Add(enumerator.Current);

        ForEachState<T> state = new ForEachState<T>();
        state.Items = items;
        state.Body = body;

        Delegate<Void, Int32> run = state.Run;
        For(0, items.Count, run);
    }

    // Runs the methods, and waits for all of them.
    public static Void Invoke(Delegate<Void>[] actions)
    {
        InvokeState state = new InvokeState();
        state.Actions = actions;

        Delegate<Void, Int32> run = state.Run;
        For(0, actions.Length, run);
    }
}

internal sealed class ForEachState<T>
{
    internal List<T> Items;
    internal Delegate<Void, T> Body;

    internal Void Run(Int32 index)
    {
        Body(Items[index]);
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
