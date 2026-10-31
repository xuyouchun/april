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
}

// MulticastDelegate<TReturn, TArgs ...>
[Internal]
public sealed class MulticastDelegate<TReturn, TArgs ...>
{
    private MulticastDelegate() { }

    public using Self = MulticastDelegate<TReturn, TArgs ...>;
    public using Delegate = Delegate<TReturn, TArgs ...>;

    private readonly Array<Delegate> __delegates;

    public Int32 Count
    {
        get { return Internal.MulticastDelegate_GetCount(); }
    }

    public Delegate this[Int32 index]
    {
        get { return Internal.MulticastDelegate_Get<Delegate>(index); }
    }

    /*
    public static Self Add(Delegate func)
    {
        return MulticastDelegate_Add<Self>(this, func);
    }

    public static Self Remove(Delegate func)
    {
        return MulticastDelegate_Remove<Self>(this, func);
    }
    */

    public TReturn Invoke(TArgs ... args)
    {
        return Internal.Delegate_Invoke<TReturn>(); 
    }
}
