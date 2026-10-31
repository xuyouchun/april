/*
 *  System.dll
 *
 *  Comparers of integers and strings.
 */

namespace Collections
{
    namespace Generic
    {
        ////////// ////////// ////////// ////////// //////////

        // Compares integers.
        public class Int32Comparer : IComparer<Int32>, IEqualityComparer<Int32>
        {
            public Int32 Compare(Int32 x, Int32 y)
            {
                if (x < y)
                    return -1;

                if (x > y)
                    return 1;

                return 0;
            }

            public Boolean Equals(Int32 x, Int32 y)
            {
                return x == y;
            }

            public Int32 GetHashCode(Int32 obj)
            {
                return obj & 2147483647;
            }
        }

        // Compares long integers.
        public class Int64Comparer : IComparer<Int64>, IEqualityComparer<Int64>
        {
            public Int32 Compare(Int64 x, Int64 y)
            {
                if (x < y)
                    return -1;

                if (x > y)
                    return 1;

                return 0;
            }

            public Boolean Equals(Int64 x, Int64 y)
            {
                return x == y;
            }

            public Int32 GetHashCode(Int64 obj)
            {
                return (Int32)(obj & 2147483647);
            }
        }

        // Compares strings, by the characters.
        public class StringComparer : IComparer<String>, IEqualityComparer<String>
        {
            public Int32 Compare(String x, String y)
            {
                return Internal.String_Compare(x, y);
            }

            public Boolean Equals(String x, String y)
            {
                return Internal.String_Equals(x, y);
            }

            public Int32 GetHashCode(String obj)
            {
                return Internal.String_GetHashCode(obj);
            }
        }
    }
}
