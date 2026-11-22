/*
 *  System.dll
 *
 *  IAsyncEnumerable<T>, IAsyncEnumerator<T>: the sequences whose next item is waited for (await foreach).
 */

using Threading.Tasks;

namespace Collections;

////////// ////////// ////////// ////////// //////////

// Goes through the items of a sequence that makes them asynchronously.
public interface IAsyncEnumerator<out T>
{
    // Goes to the next item: the task ends with false when there is no one.
    Task<Boolean> MoveNextAsync();

    // The current item.
    T Current { get; }

    // Gives back what the enumerator holds.
    Task DisposeAsync();
}

////////// ////////// ////////// ////////// //////////

// A sequence that has an enumerator that waits for the items: await foreach (T item in sequence) { ... }, and the methods that
// return it have yield return and await.
public interface IAsyncEnumerable<out T>
{
    IAsyncEnumerator<T> GetAsyncEnumerator();
}
