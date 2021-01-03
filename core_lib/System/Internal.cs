
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
	public static extern Void Console_Write(String s);

	[__internal__]
	public static extern Void Console_WriteInt32(Int32 number);
}


