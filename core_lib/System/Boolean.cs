
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

    // "True" or "False" (the case does not matter, the spaces around are skipped).
    public static Boolean Parse(String text)
    {
        Boolean result;
        if (!TryParse(text, out result))
            NumberParse.Throw(1, text, "Boolean");

        return result;
    }

    public static Boolean TryParse(String? text, out Boolean result)
    {
        result = false;
        if (text == null)
            return false;

        String lower = text.Trim().ToLower();
        if (lower == "true")
        {
            result = true;
            return true;
        }

        return lower == "false";
    }
}
