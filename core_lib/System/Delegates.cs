
/*
 *  System.dll
 *
 *  The names of the common delegates. They are aliases of Delegate<TReturn, TArgs ...> (the return type is the first one):
 *  Action<Int32, String> is Delegate<Void, Int32, String>, Func<Int32, String> is Delegate<Int32, String>.
 */


////////// ////////// ////////// ////////// //////////


// A delegate that returns nothing: Action, Action<T>, Action<T1, T2> ...
public typedef Action<TArgs ...> = Delegate<Void, TArgs ...>;

// A delegate that returns a value: Func<TRet>, Func<TRet, T>, Func<TRet, T1, T2> ... (the first argument is the one that it returns).
public typedef Func<TRet, TArgs ...> = Delegate<TRet, TArgs ...>;
