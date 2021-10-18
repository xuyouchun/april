
/* System.dll */


public static class Math
{

    ////////// ////////// ////////// ////////// //////////
    // Max

    public static Int8 Max(Int8 val1, Int8 val2)
    {
        return val1 > val2? val1 : val2;
    }

    public static Int16 Max(Int16 val1, Int16 val2)
    {
        return val1 > val2? val1 : val2;
    }

    public static Int32 Max(Int32 val1, Int32 val2)
    {
        return val1 > val2? val1 : val2;
    }

    public static Int64 Max(Int64 val1, Int64 val2)
    {
        return val1 > val2? val1 : val2;
    }

    public static UInt8 Max(UInt8 val1, UInt8 val2)
    {
        return val1 > val2? val1 : val2;
    }

    public static UInt16 Max(UInt16 val1, UInt16 val2)
    {
        return val1 > val2? val1 : val2;
    }

    public static UInt32 Max(UInt32 val1, UInt32 val2)
    {
        return val1 > val2? val1 : val2;
    }

    public static UInt64 Max(UInt64 val1, UInt64 val2)
    {
        return val1 > val2? val1 : val2;
    }

    public static Single Max(Single val1, Single val2)
    {
        return val1 > val2? val1 : val2;
    }

    public static Double Max(Double val1, Double val2)
    {
        return val1 > val2? val1 : val2;
    }

    ////////// ////////// ////////// ////////// //////////
    // Min

    public static Int8 Min(Int8 val1, Int8 val2)
    {
        return val1 < val2? val1 : val2;
    }

    public static Int16 Min(Int16 val1, Int16 val2)
    {
        return val1 < val2? val1 : val2;
    }

    public static Int32 Min(Int32 val1, Int32 val2)
    {
        return val1 < val2? val1 : val2;
    }

    public static Int64 Min(Int64 val1, Int64 val2)
    {
        return val1 < val2? val1 : val2;
    }

    public static UInt8 Min(UInt8 val1, UInt8 val2)
    {
        return val1 < val2? val1 : val2;
    }

    public static UInt16 Min(UInt16 val1, UInt16 val2)
    {
        return val1 < val2? val1 : val2;
    }

    public static UInt32 Min(UInt32 val1, UInt32 val2)
    {
        return val1 < val2? val1 : val2;
    }

    public static UInt64 Min(UInt64 val1, UInt64 val2)
    {
        return val1 < val2? val1 : val2;
    }

    public static Single Min(Single val1, Single val2)
    {
        return val1 < val2? val1 : val2;
    }

    public static Double Min(Double val1, Double val2)
    {
        return val1 < val2? val1 : val2;
    }
    
    ////////// ////////// ////////// ////////// //////////
    // Abs

    public static Int8 Abs(Int8 val)
    {
        return val < 0? (Int8)(-val) : val;
    }

    public static Int16 Abs(Int16 val)
    {
        return val < 0? (Int16)(-val) : val;
    }

    public static Int32 Abs(Int32 val)
    {
        return val < 0? -val : val;
    }

    public static Int64 Abs(Int64 val)
    {
        return val < 0? -val : val;
    }

    public static Single Abs(Single val)
    {
        return val < 0? -val : val;
    }

    public static Double Abs(Double val)
    {
        return val < 0? -val : val;
    }

    ////////// ////////// ////////// ////////// //////////
    // Sign

    public static Int32 Sign(Int8 val)
    {
        return val > 0? 1 : val < 0? -1 : 0;
    }

    public static Int32 Sign(Int16 val)
    {
        return val > 0? 1 : val < 0? -1 : 0;
    }

    public static Int32 Sign(Int32 val)
    {
        return val > 0? 1 : val < 0? -1 : 0;
    }

    public static Int32 Sign(Int64 val)
    {
        return val > 0? 1 : val < 0? -1 : 0;
    }

    public static Int32 Sign(Single val)
    {
        return val > 0? 1 : val < 0? -1 : 0;
    }

    public static Int32 Sign(Double val)
    {
        return val > 0? 1 : val < 0? -1 : 0;
    }

    ////////// ////////// ////////// ////////// //////////
    // Others.

    public static Double Sqrt(Double val)
    {
        return Internal.Math_Sqrt(val);
    }

    public static Double Pow(Double x, Double y)
    {
        return Internal.Math_Pow(x, y);
    }

    public static Double Sin(Double val)
    {
        return Internal.Math_Sin(val);
    }

    public static Double Cos(Double val)
    {
        return Internal.Math_Cos(val);
    }

    public static Double Tan(Double val)
    {
        return Internal.Math_Tan(val);
    }

    public static Double Sinh(Double val)
    {
        return Internal.Math_Sinh(val);
    }

    public static Double Cosh(Double val)
    {
        return Internal.Math_Cosh(val);
    }

    public static Double Tanh(Double val)
    {
        return Internal.Math_Tanh(val);
    }

    public static Double Asin(Double val)
    {
        return Internal.Math_Asin(val);
    }

    public static Double Acos(Double val)
    {
        return Internal.Math_Acos(val);
    }

    public static Double Atan(Double val)
    {
        return Internal.Math_Atan(val);
    }

    public static Double Asinh(Double val)
    {
        return Internal.Math_Asinh(val);
    }

    public static Double Acosh(Double val)
    {
        return Internal.Math_Acosh(val);
    }

    public static Double Atanh(Double val)
    {
        return Internal.Math_Atanh(val);
    }

    public static Double Log(Double val)
    {
        return Internal.Math_Log(val);
    }

    public static Double Log10(Double val)
    {
        return Internal.Math_Log10(val);
    }

}
