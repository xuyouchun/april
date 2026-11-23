/* EXPECT_ERROR is sealed and does not derive from */

import System;

using System;

sealed class S1 { }
interface I1 { }

class G1<T, U> where U : T { }
class G2<V> where V : I1 { }
class G3<P, Q> { }

typedef Mix<U> = G3<G1<S1, U>, G2<U>>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 1;
    }
}
