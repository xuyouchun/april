import System;
using System;
/* EXPECT_ERROR a lambda that uses local variables cannot use the members of the object */
class Calc {
 public Int32 Base = 100;
 public Delegate<Int32, Int32> Make(Int32 d) { return x => x + d + Base; }
}
class Program { [EntryPoint] public static void Main() { } }
