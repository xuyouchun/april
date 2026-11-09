
/*
 *  System.dll
 *
 *  Boolean: the members of the type (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct Boolean
{
    public Int32 CompareTo(Boolean other) => Primitives.Compare(this, other);

    public Boolean Equals(Boolean other) => Primitives.Same(this, other);

    public Boolean Equals(Object? obj) => obj is Boolean && Primitives.Same(this, (Boolean)obj);

    public Int32 GetHashCode() => Primitives.Hash(this);

    public String ToString()
    {
        if (this)
            return "True";

        return "False";
    }
}
