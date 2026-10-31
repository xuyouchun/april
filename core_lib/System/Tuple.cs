/*
 *  System.dll
 *
 *  Tuples.
 */

////////// ////////// ////////// ////////// //////////

[Internal]
public interface ITuple<TItems ...>
{
    Int32 Count { get; }

    Object Get(Int32 index);

    T Get<T>(Int32 index);
}

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Tuple<TItems ...> : ITuple<TItems ...>
{
    using __SelfType = Tuple<TItems ...>;

    public Tuple(TItems ... items)
    {
        Internal.Tuple_SetRange(typeof(__SelfType), this, items);
    }

    public Int32 Count
    {
        get { return Internal.Tuple_GetCount(typeof(__SelfType)); }
    }

    public Object Get(Int32 index)
    {
        return null;
    }

    public T Get<T>(Int32 index)
    {
        return default(T);
    }
}
