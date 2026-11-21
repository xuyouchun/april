/* EXPECT_ERROR the delegate of the property "Action" cannot be called: read it into a variable first */

import System;

using System;

class Holder
{
    private Delegate<Void> __action;

    public Delegate<Void> Action
    {
        get => __action;
        set { __action = value; }
    }
}

class Program
{
    static Void Hit() { }

    [EntryPoint]
    public static void Main()
    {
        Holder h = new Holder();
        h.Action = Hit;
        h.Action();
    }
}
