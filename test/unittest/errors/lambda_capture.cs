import System;
using System;
/* EXPECT_ERROR variable "k" undefined */
class Program { [EntryPoint] public static void Main() {
 int k = 3;
 Delegate<int, int> f = (int x) => x + k;
} }
