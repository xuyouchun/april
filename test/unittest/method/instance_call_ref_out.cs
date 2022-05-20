
// Instance call.

/* EXPECT

100
100
123
234

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Ref arguments
        {
            int value = 100;
            __WithRefArgument(ref value);

            Console.WriteLine(value);
        }

        // Out arguments
        {
            int value = 100;
            __WithOutArgument(out value);

            Console.WriteLine(value);
        }

        // Ref struct type.
        {
            Struct1 st = new Struct1();
            __RefStruct(ref st);

            Console.WriteLine(st.Value);
        }

        // Out struct type.
        {
            Struct1 st;
            __OutStruct(out st);

            Console.WriteLine(st.Value);
        }
    }

    private static void __WithRefArgument(ref int value)
    {
        value = 100;
    }

    private static void __WithOutArgument(out int value)
    {
        value = 100;
    }

    private static void __RefStruct(ref Struct1 st)
    {
        st.Value = 123;
    }

    private static void __OutStruct(out Struct1 st)
    {
        st = new Struct1();
        st.Value = 234;
    }

}


struct Struct1
{
    public int Value;
}



