/*
 *  System.dll
 *
 *  The types that HttpClient and HttpListener share: methods, status codes, headers, the content of a message.
 */

using Collections;
using IO;
using Text;
using Threading.Tasks;

namespace Net.Http;

////////// ////////// ////////// ////////// //////////

// The method of a request: GET, POST...
public class HttpMethod : IStringable
{
    private String __method;

    public HttpMethod(String method)
    {
        if (method == null || method.Length == 0)
            throw new ArgumentException("the method is empty");

        __method = method.ToUpper();
    }

    public static HttpMethod Get => new HttpMethod("GET");

    public static HttpMethod Post => new HttpMethod("POST");

    public static HttpMethod Put => new HttpMethod("PUT");

    public static HttpMethod Delete => new HttpMethod("DELETE");

    public static HttpMethod Head => new HttpMethod("HEAD");

    public static HttpMethod Options => new HttpMethod("OPTIONS");

    public static HttpMethod Patch => new HttpMethod("PATCH");

    public String Method => __method;

    public String ToString()
    {
        return __method;
    }
}

public enum HttpStatusCode
{
    OK = 200,
    Created = 201,
    Accepted = 202,
    NoContent = 204,
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    TemporaryRedirect = 307,
    PermanentRedirect = 308,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    RequestTimeout = 408,
    Conflict = 409,
    LengthRequired = 411,
    PayloadTooLarge = 413,
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
}

// An error of the protocol, or of a response that is not a success (EnsureSuccessStatusCode).
public class HttpRequestException : IOException
{
    private Int32 __status;

    public HttpRequestException(String? message) : base(message)
    {
    }

    public HttpRequestException(String? message, Int32 statusCode) : base(message)
    {
        __status = statusCode;
    }

    // The status code of the response (0 when there is none).
    public Int32 StatusCode => __status;
}

////////// ////////// ////////// ////////// //////////

// The headers of a message: the names are compared without the case, a name can have many values.
public class HttpHeaders
{
    private List<String> __names = new List<String>();
    private List<String> __values = new List<String>();

    public Int32 Count => __names.Count;

    public String NameAt(Int32 index) => __names[index];

    public String ValueAt(Int32 index) => __values[index];

    // Adds a value (the ones that the name has are kept).
    public Void Add(String name, String value)
    {
        __names.Add(name);
        __values.Add(value);
    }

    // Replaces all the values of the name by this one.
    public Void Set(String name, String value)
    {
        Remove(name);
        Add(name, value);
    }

    public Boolean Remove(String name)
    {
        Boolean removed = false;
        String lower = name.ToLower();

        for (Int32 k = __names.Count - 1; k >= 0; k--)
        {
            if (__names[k].ToLower() == lower)
            {
                __names.RemoveAt(k);
                __values.RemoveAt(k);
                removed = true;
            }
        }

        return removed;
    }

    public Boolean Contains(String name) => GetValue(name) != null;

    // The first value of the name, null if there is none.
    public String? GetValue(String name)
    {
        String lower = name.ToLower();

        for (Int32 k = 0; k < __names.Count; k++)
        {
            if (__names[k].ToLower() == lower)
                return __values[k];
        }

        return null;
    }

    // All the values of the name, joined with ", ".
    public String? GetJoined(String name)
    {
        String lower = name.ToLower();
        String? result = null;

        for (Int32 k = 0; k < __names.Count; k++)
        {
            if (__names[k].ToLower() == lower)
                result = result == null ? __values[k] : result + ", " + __values[k];
        }

        return result;
    }
}

////////// ////////// ////////// ////////// //////////

// The body of a message: bytes and the headers that tell what they are (Content-Type; Content-Length is made when it is sent).
public abstract class HttpContent
{
    private HttpHeaders __headers = new HttpHeaders();

    public HttpHeaders Headers => __headers;

    // The bytes of the body.
    public abstract UInt8[] ToArray();

    public Task<UInt8[]> ReadAsByteArrayAsync()
    {
        return Task.FromResult(ToArray());
    }

    // The text of the body: UTF-8 (a different charset of the Content-Type is not known).
    public Task<String> ReadAsStringAsync()
    {
        return Task.FromResult(Encoding.UTF8.GetString(ToArray()));
    }

    public Stream ReadAsStream()
    {
        return new MemoryStream(ToArray());
    }
}

public class ByteArrayContent : HttpContent
{
    private UInt8[] __bytes;

    public ByteArrayContent(UInt8[] bytes)
    {
        __bytes = bytes;
    }

    public override UInt8[] ToArray()
    {
        return __bytes;
    }
}

// Text, sent as UTF-8; the media type is text/plain when it is not given.
public class StringContent : HttpContent
{
    private UInt8[] __bytes;

    public StringContent(String text) : this(text, "text/plain")
    {
    }

    public StringContent(String text, String mediaType)
    {
        __bytes = Encoding.UTF8.GetBytes(text);
        Headers.Set("Content-Type", mediaType + "; charset=utf-8");
    }

    public override UInt8[] ToArray()
    {
        return __bytes;
    }
}

////////// ////////// ////////// ////////// //////////

public class HttpRequestMessage
{
    private HttpHeaders __headers = new HttpHeaders();
    private HttpMethod __method;
    private Uri __uri;
    private HttpContent? __content;

    public HttpRequestMessage(HttpMethod method, String uri)
    {
        __method = method;
        __uri = new Uri(uri);
    }

    public HttpRequestMessage(HttpMethod method, Uri uri)
    {
        __method = method;
        __uri = uri;
    }

    public HttpMethod Method
    {
        get { return __method; }
        set { __method = value; }
    }

    public Uri RequestUri
    {
        get { return __uri; }
        set { __uri = value; }
    }

    public HttpHeaders Headers => __headers;

    public HttpContent? Content
    {
        get { return __content; }
        set { __content = value; }
    }
}

public class HttpResponseMessage
{
    private HttpHeaders __headers = new HttpHeaders();
    private Int32 __status;
    private String __reason = "";
    private HttpContent __content = new ByteArrayContent(new UInt8[0]);

    public Int32 StatusCode
    {
        get { return __status; }
        set { __status = value; }
    }

    public String ReasonPhrase
    {
        get { return __reason; }
        set { __reason = value; }
    }

    public HttpHeaders Headers => __headers;

    public HttpContent Content
    {
        get { return __content; }
        set { __content = value; }
    }

    public Boolean IsSuccessStatusCode => __status >= 200 && __status <= 299;

    // Throws HttpRequestException when the status is not a success; returns the message.
    public HttpResponseMessage EnsureSuccessStatusCode()
    {
        if (!IsSuccessStatusCode)
            throw new HttpRequestException("the response status is " + __status + " " + __reason, __status);

        return this;
    }
}
