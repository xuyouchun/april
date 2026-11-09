/*
 *  System.dll
 *
 *  Delegates.
 */

using Reflection;

////////// ////////// ////////// ////////// //////////

// Delegate<TReturn, TArgs ...>
[Internal]
public struct Delegate<TReturn, TArgs ...>
{
    // Constructor.
    private Delegate(Object instance, Method method)
    {
        Internal.Delegate_Init();
    }

    private Delegate(Object instance, Method method, Int32 callType)
    {
        Internal.Delegate_InitWithCallType();
    }

    private readonly Object __instance;
    private readonly Method __method;

    public TReturn Invoke(TArgs ... args)
    {
        return Internal.Delegate_Invoke<TReturn>(); 
    }

    /// <summary>
    /// Gets whether another delegate calls the same method of the same object. Two delegates of
    /// a lambda that is made twice are not equal: each of them has its own closure.
    /// </summary>
    /// <param name="other">The other delegate.</param>
    /// <returns>True if they call the same method on the same object.</returns>
    public Boolean Equals(Delegate<TReturn, TArgs ...> other)
    {
        return Object.ReferenceEquals(__instance, other.__instance)
            && Object.ReferenceEquals(__method, other.__method);
    }
}

/// <summary>
/// The handlers of an event: a list of delegates that are called one after another. It is read
/// only: adding or removing a delegate (<c>+</c> and <c>-</c>) makes a new list, so a list that is
/// being called is never changed by a handler that is added or removed meanwhile. A list that has
/// no delegate is null, not an empty list.
/// </summary>
/// <remarks>
/// The type of an event is <c>Delegate&lt;TReturn, TArgs ...&gt;</c>; the handlers of it are kept in
/// a <c>MulticastDelegate</c> of the same type arguments.
/// </remarks>
[Internal]
public sealed class MulticastDelegate<TReturn, TArgs ...>
{
    private MulticastDelegate(Delegate<TReturn, TArgs ...>[] delegates)
    {
        __delegates = delegates;
    }

    public using Self = MulticastDelegate<TReturn, TArgs ...>;
    public using Delegate = Delegate<TReturn, TArgs ...>;

    private readonly Delegate[] __delegates;

    /// <summary>Gets the number of the delegates of the list.</summary>
    public Int32 Count => __delegates.Length;

    /// <summary>Gets the delegate at an index of the list, the first one is called first.</summary>
    public Delegate this[Int32 index] => __delegates[index];

    /// <summary>
    /// Makes a list that has the delegates of a list and a delegate after them.
    /// </summary>
    /// <param name="list">The list, or null if there is none.</param>
    /// <param name="item">The delegate that is added.</param>
    /// <returns>A new list; the list is not changed.</returns>
    public static MulticastDelegate<TReturn, TArgs ...> operator +(MulticastDelegate<TReturn, TArgs ...>? list, Delegate item)
    {
        Int32 count = list == null? 0 : list.__delegates.Length;
        Delegate[] items = new Delegate[count + 1];

        for (Int32 k = 0; k < count; k++)
            items[k] = list.__delegates[k];

        items[count] = item;
        return new Self(items);
    }

    /// <summary>
    /// Makes a list that has the delegates of a list without the last delegate that is equal to
    /// the one given (<see cref="Delegate.Equals"/>).
    /// </summary>
    /// <param name="list">The list, or null if there is none.</param>
    /// <param name="item">The delegate that is removed.</param>
    /// <returns>
    /// A new list; the same list if it does not have the delegate; null if it has no delegate
    /// after it.
    /// </returns>
    public static MulticastDelegate<TReturn, TArgs ...>? operator -(MulticastDelegate<TReturn, TArgs ...>? list, Delegate item)
    {
        if (list == null)
            return null;

        Int32 count = list.__delegates.Length;
        Int32 found = -1;

        for (Int32 k = count - 1; k >= 0 && found < 0; k--)
        {
            if (list.__delegates[k].Equals(item))
                found = k;
        }

        if (found < 0)
            return list;

        if (count == 1)
            return null;

        Delegate[] items = new Delegate[count - 1];
        for (Int32 k = 0; k < found; k++)
            items[k] = list.__delegates[k];

        for (Int32 k = found + 1; k < count; k++)
            items[k - 1] = list.__delegates[k];

        return new Self(items);
    }

    /// <summary>
    /// Calls the delegates one after another, in the order that they were added in. An exception of
    /// a delegate goes out of the call: the delegates after it are not called.
    /// </summary>
    /// <param name="args">The arguments that every delegate is called with.</param>
    /// <returns>The result of the last delegate.</returns>
    public TReturn Invoke(TArgs ... args)
    {
        Int32 last = __delegates.Length - 1;

        for (Int32 k = 0; k < last; k++)
        {
            Delegate item = __delegates[k];
            item(args ...);
        }

        Delegate lastItem = __delegates[last];
        return lastItem(args ...);
    }
}
