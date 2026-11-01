/*
 *  System.dll
 *
 *  Tuples.
 */

////////// ////////// ////////// ////////// //////////

// The tuples of any count and types: Tuple<TItems ...> is a struct, ITuple<TItems ...> is its
// interface, that is a reference type: a tuple that is made by ITuple<TItems ...>.Create is made
// in the heap, and it is never boxed.

// The base of all tuples, to use a tuple without knowing its items.
public interface ITuple
{
    Int32 Count { get; }

    // Returns the item of the index, a value is boxed.
    Object Get(Int32 index);
}

////////// ////////// ////////// ////////// //////////

[Internal]
public interface ITuple<TItems ...> : ITuple
{
    // Makes a tuple in the heap, as a reference type.
    static ITuple<TItems ...> Create(TItems ... items)
    {
        return (ITuple<TItems ...>)Internal.Tuple_Create(typeof(Tuple<TItems ...>), items);
    }
}

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Tuple<TItems ...> : ITuple<TItems ...>
{
    using __SelfType = Tuple<TItems ...>;

    public Tuple(TItems ... items)
    {
        Internal.Tuple_SetRange(typeof(__SelfType), items);
    }

    public Int32 Count
    {
        get { return Internal.Tuple_GetCount(typeof(__SelfType)); }
    }

    public Object Get(Int32 index)
    {
        if (index < 0 || index >= Count)
            throw new OutOfRangeException();

        return Internal.Tuple_Get(typeof(__SelfType), index);
    }
}
