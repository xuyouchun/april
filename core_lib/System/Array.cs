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

    // Returns length of array at specified dimension.
    public Int32 GetLength(Int32 dimension)
    {
        return Internal.Array_GetLengthOfDimension(this, dimension);
    }

    public static Array<T> New<T>(Int32 length)
    {
        return default(Array<T>);
    }
}

////////// ////////// ////////// ////////// //////////

[Internal]
public abstract class Array<T> : Array, Collections.Generic.IEnumerable<T>
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
}
