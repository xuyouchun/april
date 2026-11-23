/* EXPECT_ERROR can only be added to or removed from */

import System;

using System;
typedef Handler = System.Delegate<void, Int32>;
typedef Multi = System.MulticastDelegate<void, Int32>;

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
        Console.WriteLine(button.Clicked == null);
    }
}
