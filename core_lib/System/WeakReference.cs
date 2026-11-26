/*
 *  System.dll
 *
 *  WeakReference: a reference that does not keep the object alive.
 */

////////// ////////// ////////// ////////// //////////

// A reference to an object that does not keep it alive: when nothing else reaches the object, a collection frees it and the
// target is null. The reference itself is an object; its destructor gives the handle back.
public class WeakReference
{
    private Int64 __handle;

    public WeakReference(Object? target)
    {
        __handle = Internal.GC_WeakNew(target);
    }

    // The object, null when it was collected (or the reference has no object).
    public Object? Target
    {
        get
        {
            return Internal.GC_WeakGet(__handle);
        }

        set
        {
            Internal.GC_WeakFree(__handle);
            __handle = Internal.GC_WeakNew(value);
        }
    }

    // Whether the object is still there.
    public Boolean IsAlive => Internal.GC_WeakGet(__handle) != null;

    public ~WeakReference()
    {
        Internal.GC_WeakFree(__handle);
    }
}

////////// ////////// ////////// ////////// //////////

// A weak reference of an object of a type.
public class WeakReference<T> where T : class
{
    private Int64 __handle;

    public WeakReference(T target)
    {
        __handle = Internal.GC_WeakNew(target);
    }

    // The object when it is there.
    public Boolean TryGetTarget(out T target)
    {
        Object? o = Internal.GC_WeakGet(__handle);
        if (o == null)
        {
            target = default;
            return false;
        }

        target = (T)o;
        return true;
    }

    // The reference goes to another object.
    public Void SetTarget(T target)
    {
        Internal.GC_WeakFree(__handle);
        __handle = Internal.GC_WeakNew(target);
    }

    public ~WeakReference()
    {
        Internal.GC_WeakFree(__handle);
    }
}
