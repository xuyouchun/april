// The aliases of a library: the public ones are seen by the programs that use it, with their params and their constraints.
// The internal ones, the ones of the file and the private ones of a type are not.

import System;

using System;
using System.Collections;

typedef FileOnly = Int32;

namespace Geo
{
    public class Box<T> where T : struct
    {
        private T __v;

        public T V
        {
            get { return __v; }
            set { __v = value; }
        }
    }

    public class Pair<A, B>
    {
        private A __first;
        private B __second;

        public A First
        {
            get { return __first; }
            set { __first = value; }
        }

        public B Second
        {
            get { return __second; }
            set { __second = value; }
        }
    }

    public typedef SBox<T> = Box<T>;
    public typedef Cell = Box<Int32>;
    public typedef Entry<K> = Pair<K, String>;
    public typedef Func<TRet, TArgs...> = Delegate<TRet, TArgs...>;

    internal typedef Hidden = Box<Int64>;

    public class Holder
    {
        public typedef Items = List<Int32>;
        private typedef Secret = List<String>;

        public static Int32 Total(Items items)
        {
            Int32 total = 0;
            for (Int32 k = 0; k < items.Count; k++)
                total += items[k];

            return total;
        }

        public static Int32 Secrets()
        {
            Secret s = new Secret();
            s.Add("a");
            return s.Count;
        }

        public static Int32 Inner()
        {
            Hidden h = new Hidden();
            h.V = 3;
            FileOnly f = 4;
            return (Int32)h.V + f;
        }
    }
}
