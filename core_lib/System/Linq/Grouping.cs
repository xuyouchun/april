/*
 *  System.dll
 *
 *  System.Linq: the groups of the items that have the same key, and the lookup of them.
 */

namespace Linq;

using Collections;

////////// ////////// ////////// ////////// //////////

// The items that have a key.
public class Grouping<TKey, TElement> : IGrouping<TKey, TElement>
{
    public Grouping(TKey key)
    {
        __key = key;
        __items = new List<TElement>();
    }

    // The key of the items.
    public TKey Key => __key;

    // The number of the items.
    public Int32 Count => __items.Count;

    // Adds an item at the end.
    public Void Add(TElement item)
    {
        __items.Add(item);
    }

    public IEnumerator<TElement> GetEnumerator() => __items.GetEnumerator();

    private TKey __key;
    private List<TElement> __items;
}

////////// ////////// ////////// ////////// //////////

// The groups of items, found by their keys, in the order that the keys were added.
public class Lookup<TKey, TElement> : ILookup<TKey, TElement>
{
    public Lookup(IEqualityComparer<TKey> comparer)
    {
        __groups = new List<Grouping<TKey, TElement>>();
        __index = new Dictionary<TKey, Grouping<TKey, TElement>>(comparer);
    }

    // The number of the groups.
    public Int32 Count => __groups.Count;

    // The items that have the key: it is empty when there is no one.
    public IEnumerable<TElement> this[TKey key]
    {
        get
        {
            Grouping<TKey, TElement> group;
            if (__index.TryGetValue(key, out group))
                return group;

            return new List<TElement>();
        }
    }

    // Whether there are items that have the key.
    public Boolean Contains(TKey key) => __index.ContainsKey(key);

    // Adds an item to the group of the key, the group is made when it is the first one.
    public Void Add(TKey key, TElement item)
    {
        Grouping<TKey, TElement> group;
        if (!__index.TryGetValue(key, out group))
        {
            group = new Grouping<TKey, TElement>(key);
            __index.Add(key, group);
            __groups.Add(group);
        }

        group.Add(item);
    }

    // Goes through the groups.
    public IEnumerator<IGrouping<TKey, TElement>> GetEnumerator()
        => new LookupEnumerator<TKey, TElement>(__groups);

    private List<Grouping<TKey, TElement>> __groups;
    private Dictionary<TKey, Grouping<TKey, TElement>> __index;
}

////////// ////////// ////////// ////////// //////////

// The enumerator of the groups of a lookup: the groups are IGrouping<TKey, TElement>.
public class LookupEnumerator<TKey, TElement> : IEnumerator<IGrouping<TKey, TElement>>
{
    public LookupEnumerator(List<Grouping<TKey, TElement>> groups)
    {
        __groups = groups;
        __index = -1;
    }

    public Boolean MoveNext()
    {
        if (__index + 1 >= __groups.Count)
            return false;

        __index++;
        return true;
    }

    public IGrouping<TKey, TElement> Current => __groups[__index];

    public Void Reset()
    {
        __index = -1;
    }

    public Void Dispose()
    {
    }

    private List<Grouping<TKey, TElement>> __groups;
    private Int32 __index;
}
