/*
 *  System.dll
 *
 *  Nullable<T>: the type of int? and the others.
 */

////////// ////////// ////////// ////////// //////////

public struct Nullable<T> : IStringable
{
    // Constructor: it has a value.
    public Nullable(T value)
    {
        __hasValue = true;
        __value = value;
    }

    // Whether it has a value.
    public Boolean HasValue => __hasValue;

    // The value, it raises InvalidOperationException when there is no value.
    public T Value
    {
        get
        {
            if (!__hasValue)
                throw new InvalidOperationException("Nullable object must have a value.");

            return __value;
        }
    }

    // The value, or the default value of the type when there is no value.
    public T GetValueOrDefault() => __value;

    // The value, or the specified value when there is no value.
    public T GetValueOrDefault(T defaultValue)
    {
        if (__hasValue)
            return __value;

        return defaultValue;
    }

    // The text of the value, an empty string when there is no value.
    public String ToString()
    {
        if (!__hasValue)
            return "";

        Object boxed = __value;
        return Object.ToString(boxed);
    }

    private Boolean __hasValue;
    private T __value;
}
