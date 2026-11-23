// What a library does not give: an internal alias, an alias of a file, a private alias of a type, and a type that does not
// satisfy the constraints of a public alias.

import System;
import lib;

using System;
using lib.Geo;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Hidden h = null;
        FileOnly f = 1;
        Holder.Secret s = null;
        SBox<String> b = null;
    }
}
