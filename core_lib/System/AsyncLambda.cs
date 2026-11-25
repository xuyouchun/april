/*
 *  System.dll
 *
 *  The async lambda: async () => body is () => AsyncLambda.Run(() => body). The body runs in a task of the thread pool, the awaits in
 *  it wait in that thread.
 */

using Threading.Tasks;

////////// ////////// ////////// ////////// //////////

/// <summary>What an async lambda calls (the compiler writes the call).</summary>
public static class AsyncLambda
{
    /// <summary>Runs the body, that has no result.</summary>
    public static Task Run(Delegate<Void> body)
    {
        return Task.Run(body);
    }

    /// <summary>Runs the body, that has a result.</summary>
    public static Task<T> Run<T>(Delegate<T> body)
    {
        return Task.Run<T>(body);
    }
}
