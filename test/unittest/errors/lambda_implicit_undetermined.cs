import System;
using System;
/* EXPECT_ERROR the type of the lambda cannot be determined */
class Program { [EntryPoint] public static void Main() {
 var f = x => x;
} }
