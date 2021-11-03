/* System.dll */


public struct DateTime
{
    ////////// ////////// ////////// ////////// //////////

    public DateTime(Int64 ticks)
    {
        __ticks = ticks;
    }

    public Int64 Ticks { get { return __ticks; } }

    public static DateTime UtcNow
    {
        get
        {
            return new DateTime(12345);
        }
    }

    public static Void SetUtcNow(DateTime dt)
    {
        Console.WriteLine(dt.__ticks);
    }
    
    private readonly Int64 __ticks;

    ////////// ////////// ////////// ////////// //////////
}

