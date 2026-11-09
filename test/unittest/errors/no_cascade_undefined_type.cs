/* EXPECT_ERROR type "Foo" is not defined */
/* EXPECT_CODE AP3201 */
/* EXPECT_NOT (invalid expression|cannot determine|is not defined in type) */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Foo f = new Foo();
        int y = f.Bar(1);
    }
}
