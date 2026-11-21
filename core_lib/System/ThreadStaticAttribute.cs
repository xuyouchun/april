/*
 *  System.dll
 *
 *  ThreadStaticAttribute: a static field that each thread has its own copy of.
 */

////////// ////////// ////////// ////////// //////////

// Marks a static field: each thread reads and writes its own value of it (it is the default of the type for a thread that did
// not write it: the initial value of the field is for the thread that runs the static constructor, as in .NET).
[AttributeUsage]
public class ThreadStaticAttribute : Attribute
{
    public ThreadStaticAttribute()
    {
    }
}
