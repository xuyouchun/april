import System;
using System;
/* EXPECT_ERROR goto statement is not supported */
class Program { [EntryPoint] public static void Main() {
int x = 0; goto end; x = 1;
} }
