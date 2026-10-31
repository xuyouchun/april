import System;
using System;
/* EXPECT_ERROR not defined for */
class V { public int X; }
class Program { [EntryPoint] public static void Main() {
V a = new V(); V b = new V();
V c = a + b;
} }
