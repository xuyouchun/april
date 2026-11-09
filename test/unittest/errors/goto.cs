import System;
using System;
/* EXPECT_ERROR "goto" is not supported */
class Program { [EntryPoint] public static void Main() {
int x = 0; goto end; x = 1;
} }
