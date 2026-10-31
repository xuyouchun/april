/*
 *  System.dll
 *
 *  The enumerators of the collections, that foreach uses.
 */

namespace Collections.Generic;

////////// ////////// ////////// ////////// //////////

// The enumerator of a list.
public class ListEnumerator<T> : IEnumerator<T>
{
    public ListEnumerator(List<T> list)
    {
        __list = list;
        __index = -1;
    }

    // Goes to the next item, returns false at the end.
    public Boolean MoveNext()
    {
        if (__index + 1 >= __list.__count)
            return false;

        __index++;
        return true;
    }

    // The current item.
    public T Current
    {
        get { return __list.__items[__index]; }
    }

    // Goes back to before the first item.
    public Void Reset()
    {
        __index = -1;
    }

    // Nothing to release.
    public Void Dispose()
    {
    }

    private List<T> __list;
    private Int32 __index;
}

////////// ////////// ////////// ////////// //////////

// The enumerator of an array.
public class ArrayEnumerator<T> : IEnumerator<T>
{
    public ArrayEnumerator(T[] items)
    {
        __items = items;
        __index = -1;
    }

    public Boolean MoveNext()
    {
        if (__index + 1 >= __items.Length)
            return false;

        __index++;
        return true;
    }

    public T Current
    {
        get { return __items[__index]; }
    }

    public Void Reset()
    {
        __index = -1;
    }

    // Nothing to release.
    public Void Dispose()
    {
    }

    private T[] __items;
    private Int32 __index;
}
