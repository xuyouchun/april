import System;
using System;
/* EXPECT_ERROR the local variable "a" is used by a lambda, it cannot be modified */
class Program { [EntryPoint] public static void Main() {
 Int32 a = 1;
 Delegate<Int32> f = () => a;
 a = 5;
} }
