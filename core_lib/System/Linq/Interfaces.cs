/*
 *  System.dll
 *
 *  System.Linq: the interfaces of the results of the queries, like those of .NET.
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

// A group of items that have a key: an item of the result of GroupBy.
public interface IGrouping<out TKey, out TElement> : IEnumerable<TElement>
{
    // The key of the group.
    TKey Key { get; }
}

////////// ////////// ////////// ////////// //////////

// A collection of keys, each one has the group of the items that have it.
public interface ILookup<TKey, TElement> : IEnumerable<IGrouping<TKey, TElement>>
{
    // The number of the groups.
    Int32 Count { get; }

    // The items that have the key: it is empty when there is no one.
    IEnumerable<TElement> this[TKey key] { get; }

    // Whether there are items that have the key.
    Boolean Contains(TKey key);
}

////////// ////////// ////////// ////////// //////////

// The items of a collection that are ordered: ThenBy orders the items that are equal by the
// keys before it.
public interface IOrderedEnumerable<out T> : IEnumerable<T>
{
    // Makes the collection that is ordered by the key, after the keys of this one.
    IOrderedEnumerable<T> CreateOrderedEnumerable<TKey>(Delegate<TKey, T> keySelector,
                                                        IComparer<TKey> comparer,
                                                        Boolean descending);
}
