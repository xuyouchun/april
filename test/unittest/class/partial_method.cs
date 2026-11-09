// Partial methods: the declaration has no body. With an implementation in another part it is
// the method, without it the calls do nothing.

/* EXPECT

changing 5
5
done

*/

import System;

using System;

partial class Model
{
    public int Value;

    partial void OnChanging(int v);

    partial void OnChanged(int v);

    public void Set(int v)
    {
        this.OnChanging(v);
        this.Value = v;
        this.OnChanged(v);
    }
}

partial class Model
{
    partial void OnChanging(int v)
    {
        Console.WriteLine("changing " + v);
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Model m = new Model();
        m.Set(5);
        Console.WriteLine(m.Value);
        Console.WriteLine("done");
    }
}
