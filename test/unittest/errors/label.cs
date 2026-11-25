import System;
using System;
/* EXPECT_ERROR the label "end" is defined twice in the method */
class Program { [EntryPoint] public static void Main() {
int x = 0;
end: x = 1;
end: x = 2;
} }
