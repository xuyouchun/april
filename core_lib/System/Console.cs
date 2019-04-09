
/*
 *  System.dll
 *
 */


////////// ////////// ////////// ////////// //////////


public static class Console
{
    [__internal__]
    public static extern Int32 TestAdd(Int32 a, Int32 b);

    [__internal__]
    public static extern Void Write(String s);

    [__internal__]
    public static extern Void WriteInt32(Int32 v);

    public static Void WriteLine(String s)
    {
        Write(s);
        WriteLine();
    }

	public static Void WriteLine()
    {
        Write("\n");
    }

    public static Void Print(Int32 v)
    {
        Write("Result: ");
        WriteInt32(v);
        WriteLine();
    }
};








