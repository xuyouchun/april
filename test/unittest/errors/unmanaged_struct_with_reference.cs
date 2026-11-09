/* EXPECT_ERROR (unmanaged) */

import System;

using System;

struct Named
{
    public Int32 Id;
    public String Name;
}

class Holder<T> where T : unmanaged
{
    public T Value;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Holder<Named> holder = new Holder<Named>();
        Console.WriteLine(holder.Value.Id);
    }
}
