/*
 *  System.dll
 *
 *  TaskContinuationOptions.
 */

namespace Threading.Tasks;

////////// ////////// ////////// ////////// //////////

// When a continuation (Task.ContinueWith) runs, and where. A continuation whose condition is not true does not run, and its task is
// canceled. The values are those of .NET: the conditions can be put together with |.
public enum TaskContinuationOptions
{
    None = 0,
    LongRunning = 2,                        // (no effect: the pool makes threads when the work waits)
    NotOnRanToCompletion = 0x10000,
    NotOnFaulted = 0x20000,
    OnlyOnCanceled = 0x30000,
    NotOnCanceled = 0x40000,
    OnlyOnFaulted = 0x50000,
    OnlyOnRanToCompletion = 0x60000,
    ExecuteSynchronously = 0x80000,         // In the thread that ends the task, not in the pool.
}
