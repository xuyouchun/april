import System;
using System;
/* EXPECT_ERROR foreach is not supported for type */
class Program { [EntryPoint] public static void Main() {
int n = 5;
foreach (int x in n) { Console.WriteLine(x); }
} }
