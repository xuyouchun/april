
/*
 *  System.dll
 *
 */


////////// ////////// ////////// ////////// //////////


public static class Console
{
    public static Void Write(String value)
	{
		Internal.Console_WriteString(value);
	}

    public static Void WriteLine(String value)
    {
        Write(value);
        WriteLine();
    }

	public static Void WriteLine()
    {
        Write("\n");
    }

	public static Void Write(Char value)
	{
		Internal.Console_WriteChar(value);
	}

    public static Void Write(Int8 value)
	{
		Internal.Console_WriteInt8(value);
	}

    public static Void Write(UInt8 value)
	{
		Internal.Console_WriteUInt8(value);
	}

    public static Void Write(Int16 value)
	{
		Internal.Console_WriteInt16(value);
	}

    public static Void Write(UInt16 value)
	{
		Internal.Console_WriteUInt16(value);
	}

    public static Void Write(Int32 value)
	{
		Internal.Console_WriteInt32(value);
	}

    public static Void Write(UInt32 value)
	{
		Internal.Console_WriteUInt32(value);
	}

	public static Void Write(Int64 value)
	{
		Internal.Console_WriteInt64(value);
	}

	public static Void Write(UInt64 value)
	{
		Internal.Console_WriteUInt64(value);
	}

	public static Void Write(Single value)
	{
		Internal.Console_WriteSingle(value);
	}

	public static Void Write(Double value)
	{
		Internal.Console_WriteDouble(value);
	}
};








