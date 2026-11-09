/* EXPECT_ERROR has no field or property "Missing" to assign */

import System;

using System;

class TagAttribute : Attribute
{
    public Int32 Level;

    public TagAttribute(String name) { }
}

[Tag("a", Missing = 3)]
class Doc { }

class Program
{
    [EntryPoint]
    public static void Main() { }
}
