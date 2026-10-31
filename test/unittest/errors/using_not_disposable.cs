import System;
using System;
/* EXPECT_ERROR using requires a type with Dispose\(\) */
class P { }
class Program { [EntryPoint] public static void Main() {
 using (P p = new P()) { Console.WriteLine("x"); }
} }
