/*
 *  System.dll
 *
 *  GC: the garbage collector.
 */

////////// ////////// ////////// ////////// //////////

// The garbage collector. The objects that nothing reaches are freed by it (a collection starts when the program has
// allocated some megabytes since the last one), and the objects that have a destructor are not freed before the destructor
// has run: it runs soon after the collection that found the object, on the thread that runs the program, when the program
// calls a method or goes back in a loop. There is one generation.
public static class GC
{
    // Collects the garbage now.
    public static Void Collect()
    {
        Internal.GC_Collect();
    }

    // The number of collections that were made.
    public static Int32 CollectionCount() => Internal.GC_CollectionCount();

    // The number of collections that were made (there is one generation).
    public static Int32 CollectionCount(Int32 generation) => Internal.GC_CollectionCount();

    // The bytes of the objects that are in use (the garbage that is not collected yet included); a collection is made
    // first when `forceFullCollection` is true.
    public static Int64 GetTotalMemory(Boolean forceFullCollection)
    {
        if (forceFullCollection)
            Internal.GC_Collect();

        return Internal.GC_TotalMemory();
    }

    // The number of the last generation.
    public static Int32 MaxGeneration => 0;

    // The generation of the object.
    public static Int32 GetGeneration(Object obj) => 0;

    // The object is alive until this call (the variables of a method keep what they hold until the method ends: this call
    // makes it clear in the code).
    public static Void KeepAlive(Object? obj)
    {
    }

    // The destructors of the objects that a collection found run now. (They run at the next call of a method or jump back of
    // the program anyway; this call is that point: the thread that calls it runs them before it goes on.)
    public static Void WaitForPendingFinalizers()
    {
    }

    // The destructor of the object does not run.
    public static Void SuppressFinalize(Object obj)
    {
        Internal.GC_SuppressFinalize(obj, true);
    }

    // The destructor of the object runs again, when it was suppressed.
    public static Void ReRegisterForFinalize(Object obj)
    {
        Internal.GC_SuppressFinalize(obj, false);
    }
}
