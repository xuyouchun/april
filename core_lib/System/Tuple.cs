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
    // Returns the count of the items.
    Int32 GetSize();

    // Returns the item of the index, a value is boxed.
    Object Get(Int32 index);

    // Makes a tuple in the heap, as a reference type: it is never boxed.
    public static ITuple<TItems ...> Create<TItems ...>(TItems ... items)
        => (ITuple<TItems ...>)Internal.Tuple_Create(typeof(Tuple<TItems ...>), items);
}

////////// ////////// ////////// ////////// //////////

[Internal]
public interface ITuple<TItems ...> : ITuple
{
    // Empty.
}

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Tuple<TItems ...> : ITuple<TItems ...>, IStringable
{
    typedef __SelfType = Tuple<TItems ...>;

    public Tuple(TItems ... items)
    {
        Internal.Tuple_SetRange(typeof(__SelfType), items);
    }

    public Int32 GetSize() => Internal.Tuple_GetCount(typeof(__SelfType));

    public Object Get(Int32 index)
    {
        if (index < 0 || index >= GetSize())
            throw new OutOfRangeException();

        return Internal.Tuple_Get(typeof(__SelfType), index);
    }

    // The tuples are equal when the items are, one by one.
    public Boolean Equals(Object? obj)
    {
        ITuple? other = obj as ITuple;
        if (other == null || other.GetSize() != GetSize())
            return false;

        for (Int32 k = 0; k < GetSize(); k++)
        {
            if (!Object.Equals(Get(k), other.Get(k)))
                return false;
        }

        return true;
    }

    // The hash code is made of the items.
    public Int32 GetHashCode()
    {
        Int32 hash = 17;

        for (Int32 k = 0; k < GetSize(); k++)
        {
            hash = hash * 31 + Object.HashOf(Get(k));
        }

        return hash;
    }

    // (1, x)
    public String ToString()
    {
        String text = "(";

        for (Int32 k = 0; k < GetSize(); k++)
        {
            if (k > 0)
                text += ", ";

            Object item = Get(k);
            text += item == null? "" : Object.ToString(item);
        }

        return text + ")";
    }
}
