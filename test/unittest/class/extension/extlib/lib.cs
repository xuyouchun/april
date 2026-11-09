// The library of the test of the extension methods that are in another assembly.

import System;

using System;

public static class Text
{
    public static String Shout(this String text)
    {
        return text + "!";
    }

    public static int Twice(this int value)
    {
        return value * 2;
    }
}

// Not an extension method: the class has no mark.
public static class Plain
{
    public static int Twice(int value)
    {
        return value * 3;
    }
}
