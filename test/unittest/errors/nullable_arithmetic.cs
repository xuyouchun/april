import System;
using System;
/* EXPECT_ERROR cannot implicitly convert type "String" to "Nullable<Int32>" */
class Program { [EntryPoint] public static void Main() {
int? a = 1;
int? b = a + "x";
} }
