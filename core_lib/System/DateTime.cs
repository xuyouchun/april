/* System.dll */


public struct DateTime
{
    ////////// ////////// ////////// ////////// //////////

    public DateTime(Int64 ticks)
    {
        __ticks = ticks;
    }

    public static DateTime UtcNow
    {
        get
        {
            return new DateTime(0);
        }
    }
    
    private readonly Int64 __ticks;

    ////////// ////////// ////////// ////////// //////////
}

