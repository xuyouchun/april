/*
 *  System.dll
 *
 *  Uri: an absolute address "http://host:port/path?query".
 */

namespace Net;

////////// ////////// ////////// ////////// //////////

// An address of the web. Only http is known (the port is 80 when it is not written); a relative address is resolved against
// another one with Uri(Uri, String).
public class Uri : IStringable
{
    private String __scheme;
    private String __host;
    private Int32 __port;
    private String __pathAndQuery;

    public Uri(String text)
    {
        if (!__Parse(text))
            throw new FormatException("'" + text + "' is not an absolute http address");
    }

    // The address that the text is, or the one that it is when it is relative to the base address.
    public Uri(Uri baseUri, String relative)
    {
        if (relative.StartsWith("http://") || relative.StartsWith("https://"))
        {
            if (!__Parse(relative))
                throw new FormatException("'" + relative + "' is not an absolute http address");

            return;
        }

        String text;
        if (relative.StartsWith("/"))
            text = baseUri.Authority + relative;
        else
            text = baseUri.Authority + __Directory(baseUri.PathAndQuery) + relative;

        if (!__Parse(text))
            throw new FormatException("'" + relative + "' is not a valid address");
    }

    private Boolean __Parse(String text)
    {
        if (text.StartsWith("https://"))
            throw new NotSupportedException("https is not supported (no TLS)");

        if (!text.StartsWith("http://"))
            return false;

        String rest = text.Substring(7);

        Int32 slash = rest.IndexOf('/');
        Int32 question = rest.IndexOf('?');
        if (slash < 0 || (question >= 0 && question < slash))
        {
            // "http://host?query": the path is "/".
            Int32 cut = slash < 0 && question < 0 ? rest.Length : (question >= 0 ? question : slash);
            __pathAndQuery = "/" + rest.Substring(cut);
            rest = rest.Substring(0, cut);
        }
        else
        {
            __pathAndQuery = rest.Substring(slash);
            rest = rest.Substring(0, slash);
        }

        Int32 hash = __pathAndQuery.IndexOf('#');
        if (hash >= 0)
            __pathAndQuery = __pathAndQuery.Substring(0, hash);

        Int32 colon = rest.LastIndexOf(':');
        if (colon >= 0)
        {
            __host = rest.Substring(0, colon);
            Int32 port = 0;
            String digits = rest.Substring(colon + 1);

            if (digits.Length == 0 || digits.Length > 5)
                return false;

            for (Int32 k = 0; k < digits.Length; k++)
            {
                Int32 d = (Int32)digits[k] - 48;
                if (d < 0 || d > 9)
                    return false;

                port = port * 10 + d;
            }

            if (port > 65535)
                return false;

            __port = port;
        }
        else
        {
            __host = rest;
            __port = 80;
        }

        __scheme = "http";
        return __host.Length > 0;
    }

    // The path of the directory of a path: "/a/b/c?x" -> "/a/b/".
    private static String __Directory(String pathAndQuery)
    {
        Int32 question = pathAndQuery.IndexOf('?');
        String path = question < 0 ? pathAndQuery : pathAndQuery.Substring(0, question);

        return path.Substring(0, path.LastIndexOf('/') + 1);
    }

    public String Scheme => __scheme;

    public String Host => __host;

    public Int32 Port => __port;

    public String PathAndQuery => __pathAndQuery;

    // The path without the query.
    public String AbsolutePath
    {
        get
        {
            Int32 question = __pathAndQuery.IndexOf('?');
            return question < 0 ? __pathAndQuery : __pathAndQuery.Substring(0, question);
        }
    }

    // The query with its "?" (empty when there is none).
    public String Query
    {
        get
        {
            Int32 question = __pathAndQuery.IndexOf('?');
            return question < 0 ? "" : __pathAndQuery.Substring(question);
        }
    }

    // "http://host:port" (the port is not written when it is 80).
    public String Authority => __scheme + "://" + __host + (__port == 80 ? "" : ":" + __port);

    public String AbsoluteUri => Authority + __pathAndQuery;

    public String ToString()
    {
        return AbsoluteUri;
    }
}
