// EXPECT_WARNING 4009 4009 4009

/* EXPECT

*/

#nullable enable

import System;

using System;

// A field that cannot be null has no value: the default constructor.
class NoConstructor
{
    public String Name;                 // 4009
    public String? Optional;
    public String Title = "t";
    public int N;
}

// The constructors set the fields before they exit.
class Constructors
{
    public String First;
    public String Second;               // 4009: the constructor without params does not set it

    public Constructors(String s)
    {
        First = s;
        Second = s;
    }

    public Constructors()
    {
        this.First = "x";
    }
}

// A return, a branch, and this(...).
class Flow
{
    public String Name;

    public Flow(bool b)
    {
        if (b)
        {
            Name = "a";
            return;
        }

        Name = "b";
    }

    public Flow(int k) : this(true)
    {
    }
}

class Branch
{
    public String Name;                 // 4009: the other branch does not set it

    public Branch(bool b)
    {
        if (b)
            Name = "a";
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
    }
}
