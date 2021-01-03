
/*
 *  System.dll
 *
 */


////////// ////////// ////////// ////////// //////////


public static class Console
{
    public static Void Write(String s)
	{
		Internal.Console_Write(s);
	}

    public static Void WriteLine(String s)
    {
        Write(s);
        WriteLine();
    }

	public static Void WriteLine()
    {
        Write("\n");
    }

    public static Void Write(Int32 value)
	{
		// Internal.Console_WriteInt32(value);
	}
};








