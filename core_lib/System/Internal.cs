
/*
 *  System.dll
 *
 */

////////// ////////// ////////// ////////// //////////

internal static class Internal
{

    // Object

    [__internal__]
    public static extern Object Object_FromPtr(Ptr ptr);


    // Array

    [__internal__]
    public static extern Int32 Array_GetRank(Array array);

    [__internal__]
    public static extern Int32 Array_GetLength(Array array);

    [__internal__]
    public static extern Int32 Array_GetLengthOfDimension(Array array, Int32 dimension);


    // Tuple

    [__internal__]
    public static extern Void Tuple_SetRange(Object tuple, Ptr data);

    [__internal__]
    public static extern Ptr Tuple_Get(Object tuple, Int32 index);

    [__internal__]
    public static extern Int32 Tuple_GetCount(Object tuple);


    // Console

    [__internal__]
    public static extern Void Console_WriteString(String value);

    [__internal__]
    public static extern Void Console_WriteStringLine(String value);

    [__internal__]
    public static extern Void Console_WriteChar(Char value);

    [__internal__]
    public static extern Void Console_WriteInt8(Int8 value);

    [__internal__]
    public static extern Void Console_WriteUInt8(UInt8 value);

    [__internal__]
    public static extern Void Console_WriteInt16(Int16 value);

    [__internal__]
    public static extern Void Console_WriteUInt16(UInt16 value);

    [__internal__]
    public static extern Void Console_WriteInt32(Int32 value);

    [__internal__]
    public static extern Void Console_WriteUInt32(UInt32 value);

    [__internal__]
    public static extern Void Console_WriteInt64(Int64 value);

    [__internal__]
    public static extern Void Console_WriteUInt64(UInt64 value);

    [__internal__]
    public static extern Void Console_WriteSingle(Single value);

    [__internal__]
    public static extern Void Console_WriteDouble(Double value);

    [__internal__]
    public static extern Void Console_WriteStringLine(String value);

    [__internal__]
    public static extern Void Console_WriteCharLine(Char value);

    [__internal__]
    public static extern Void Console_WriteInt8Line(Int8 value);

    [__internal__]
    public static extern Void Console_WriteUInt8Line(UInt8 value);

    [__internal__]
    public static extern Void Console_WriteInt16Line(Int16 value);

    [__internal__]
    public static extern Void Console_WriteUInt16Line(UInt16 value);

    [__internal__]
    public static extern Void Console_WriteInt32Line(Int32 value);

    [__internal__]
    public static extern Void Console_WriteUInt32Line(UInt32 value);

    [__internal__]
    public static extern Void Console_WriteInt64Line(Int64 value);

    [__internal__]
    public static extern Void Console_WriteUInt64Line(UInt64 value);

    [__internal__]
    public static extern Void Console_WriteSingleLine(Single value);

    [__internal__]
    public static extern Void Console_WriteDoubleLine(Double value);


    // Reflection

    [__internal__]
    public static extern Type Reflection_GetType(Object obj);

    [__internal__]
    public static extern String Reflection_GetTypeName(Object obj);


    // Delegate
    [__internal__]
    public static extern Void Delegate_Invoke();

    // Math

    [__internal__]
    public static extern Double Math_Sqrt(Double val);

    [__internal__]
    public static extern Double Math_Pow(Double x, Double y);

    [__internal__]
    public static extern Double Math_Sin(Double val);

    [__internal__]
    public static extern Double Math_Cos(Double val);

    [__internal__]
    public static extern Double Math_Tan(Double val);

    [__internal__]
    public static extern Double Math_Sinh(Double val);

    [__internal__]
    public static extern Double Math_Cosh(Double val);

    [__internal__]
    public static extern Double Math_Tanh(Double val);

    [__internal__]
    public static extern Double Math_Asin(Double val);

    [__internal__]
    public static extern Double Math_Acos(Double val);

    [__internal__]
    public static extern Double Math_Atan(Double val);

    [__internal__]
    public static extern Double Math_Asinh(Double val);

    [__internal__]
    public static extern Double Math_Acosh(Double val);

    [__internal__]
    public static extern Double Math_Atanh(Double val);

    [__internal__]
    public static extern Double Math_Log(Double val);

    [__internal__]
    public static extern Double Math_Log10(Double val);
}


