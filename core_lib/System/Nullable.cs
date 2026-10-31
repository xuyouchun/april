/*
 *  System.dll
 *
 *  Nullable<T>: the type of int? and the others.
 */

////////// ////////// ////////// ////////// //////////

public struct Nullable<T>
{
    // Constructor: it has a value.
    public Nullable(T value)
    {
        __hasValue = true;
        __value = value;
    }

    // Whether it has a value.
    public Boolean HasValue
    {
        get { return __hasValue; }
    }

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
    public T GetValueOrDefault()
    {
        return __value;
    }

    // The value, or the specified value when there is no value.
    public T GetValueOrDefault(T defaultValue)
    {
        if (__hasValue)
            return __value;

        return defaultValue;
    }

    private Boolean __hasValue;
    private T __value;
}
