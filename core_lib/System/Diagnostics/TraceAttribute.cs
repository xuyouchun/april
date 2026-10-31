/*
 *  System.dll
 *
 */

////////// ////////// ////////// ////////// //////////

namespace Diagnostics
{
    // Trace Attribute.
    [Internal, AttributeUsage]
    public class TraceAttribute : Attribute
    {
        public TraceAttribute()
        {

        }
    };
}
