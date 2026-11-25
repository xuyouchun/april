import System;
using System;
/* EXPECT_ERROR the label "end" is not defined in the method */
class Program { [EntryPoint] public static void Main() {
int x = 0; goto end; x = 1;
} }
