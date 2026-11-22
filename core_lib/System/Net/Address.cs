/*
 *  System.dll
 *
 *  IPAddress, IPEndPoint, Dns: the addresses of the network (IPv4).
 */

using Net.Sockets;
using Threading.Tasks;

namespace Net;

////////// ////////// ////////// ////////// //////////

// An IPv4 address.
public class IPAddress : IStringable, IEquality
{
    private UInt8[] __bytes;

    public IPAddress(UInt8[] address)
    {
        if (address == null || address.Length != 4)
            throw new ArgumentException("an IPv4 address has 4 bytes");

        __bytes = new UInt8[4];
        for (Int32 k = 0; k < 4; k++)
            __bytes[k] = address[k];
    }

    // 127.0.0.1, 0.0.0.0 (any of the addresses of the machine).
    public static IPAddress Loopback => new IPAddress(new UInt8[] { 127, 0, 0, 1 });

    public static IPAddress Any => new IPAddress(new UInt8[] { 0, 0, 0, 0 });

    public UInt8[] GetAddressBytes()
    {
        return new UInt8[] { __bytes[0], __bytes[1], __bytes[2], __bytes[3] };
    }

    // "a.b.c.d": four numbers from 0 to 255.
    public static Boolean TryParse(String? text, out IPAddress? address)
    {
        address = null;
        if (text == null)
            return false;

        UInt8[] bytes = new UInt8[4];
        Int32 part = 0, value = 0, digits = 0;

        for (Int32 k = 0; k <= text.Length; k++)
        {
            Char c = k < text.Length ? text[k] : '.';

            if (c >= '0' && c <= '9')
            {
                value = value * 10 + ((Int32)c - 48);
                digits = digits + 1;

                if (value > 255 || digits > 3)
                    return false;
            }
            else if (c == '.' && digits > 0 && part < 4)
            {
                bytes[part] = (UInt8)value;
                part = part + 1;
                value = 0;
                digits = 0;

                if (k == text.Length && part != 4)
                    return false;
            }
            else
            {
                return false;
            }
        }

        if (part != 4)
            return false;

        address = new IPAddress(bytes);
        return true;
    }

    public static IPAddress Parse(String text)
    {
        IPAddress? address;
        if (!TryParse(text, out address))
            throw new FormatException("'" + text + "' is not an IPv4 address");

        return address;
    }

    public String ToString()
    {
        return __bytes[0] + "." + __bytes[1] + "." + __bytes[2] + "." + __bytes[3];
    }

    public Boolean Equals(Object? other)
    {
        IPAddress? address = other as IPAddress;
        if (address == null)
            return false;

        for (Int32 k = 0; k < 4; k++)
        {
            if (__bytes[k] != address.__bytes[k])
                return false;
        }

        return true;
    }

    public Int32 GetHashCode()
    {
        return ((Int32)__bytes[0] << 24) | ((Int32)__bytes[1] << 16) | ((Int32)__bytes[2] << 8) | (Int32)__bytes[3];
    }
}

////////// ////////// ////////// ////////// //////////

// An address and a port.
public class IPEndPoint : IStringable
{
    private IPAddress __address;
    private Int32 __port;

    public IPEndPoint(IPAddress address, Int32 port)
    {
        if (port < 0 || port > 65535)
            throw new ArgumentException("the port is not from 0 to 65535");

        __address = address;
        __port = port;
    }

    public IPAddress Address => __address;

    public Int32 Port => __port;

    public String ToString()
    {
        return __address.ToString() + ":" + __port;
    }
}

////////// ////////// ////////// ////////// //////////

// The names of the machines.
public static class Dns
{
    // The first IPv4 address of a name (an address as text is the address); SocketException if the name is not known.
    public static IPAddress GetHostAddress(String host)
    {
        IPAddress? address;
        if (IPAddress.TryParse(host, out address))
            return address;

        String? text = Internal.Net_Resolve(host);
        if (text == null || !IPAddress.TryParse(text, out address))
            throw new SocketException(-4, "the name '" + host + "' is not known");

        return address;
    }

    // The same, in the thread pool.
    public static Task<IPAddress> GetHostAddressAsync(String host)
    {
        DnsWork work = new DnsWork(host);
        Delegate<IPAddress> run = work.Run;

        return await Task.Run(run);
    }
}

internal class DnsWork
{
    private String __host;

    internal DnsWork(String host)
    {
        __host = host;
    }

    internal IPAddress Run()
    {
        return Dns.GetHostAddress(__host);
    }
}
