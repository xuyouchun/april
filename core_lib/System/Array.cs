/*
 *  System.dll
 *
 *  Arrays.
 */

////////// ////////// ////////// ////////// //////////

[Internal]
public abstract class Array : Object
{
    // Returns rank of array.
    public Int32 Rank { get { return Internal.Array_GetRank(this); } }

    // Returns length of array.
    public Int32 Length { get { return Internal.Array_GetLength(this); } }

    // Returns length of array at specified dimension, OutOfRangeException if the array has not
    // the dimension.
    public Int32 GetLength(Int32 dimension)
    {
        if (dimension < 0 || dimension >= this.Rank)
            throw new OutOfRangeException();

        return Internal.Array_GetLengthOfDimension(this, dimension);
    }

    public static Array<T> New<T>(Int32 length) => default(Array<T>);
}

////////// ////////// ////////// ////////// //////////

// The base type of the arrays of T: T[], T[,] and the others are derived from it, an array is
// one-dimensional for it: the items are in the order of the rows (the last index changes first).
// An array is a collection that is read by the interface: it has the count, the indexer by one
// index, and the enumerator of the items. (The foreach of an array is made by the compiler.)
[Internal]
public abstract class Array<T> : Array, Collections.IReadOnlyList<T>
{
    // Gets/Sets value of specified index.
    public T this[Int32 index]
    {
        get
        {
            return Internal.Array_GetValue<T>(index, this);
        }
        set
        {
            Internal.Array_SetValue<T>(value, index, this);
        }
    }

    // The number of the items.
    Int32 Collections.IReadOnlyCollection<T>.Count
    {
        get => this.Length;
    }

    // The enumerator of the items.
    Collections.IEnumerator<T> Collections.IEnumerable<T>.GetEnumerator()
    {
        return new Collections.ArrayEnumerator<T>(this);
    }
}
