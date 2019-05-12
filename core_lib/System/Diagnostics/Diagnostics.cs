/*
 *  System.dll
 *
 */

////////// ////////// ////////// ////////// //////////

namespace Diagnostics
{
    // Trace Attribute.
    [__internal__, AttributeUsage]
    public class TraceAttribute : Attribute
    {
        public TraceAttribute()
        {

        }
    };
}
