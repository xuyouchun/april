import System;
using System;
/* EXPECT_ERROR a method that has yield statements returns IEnumerable<T> or IEnumerator<T> */
class Program {
 static int F() { yield return 1; }
 [EntryPoint] public static void Main() { }
}
