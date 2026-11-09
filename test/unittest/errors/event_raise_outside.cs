/* EXPECT_ERROR inaccessible due to its protection level */

import System;

using System;
using Handler = System.Delegate<void, Int32>;
using Multi = System.MulticastDelegate<void, Int32>;

class Button
{
    public event Handler Clicked;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Button button = new Button();
        button.Clicked(1);
    }
}
