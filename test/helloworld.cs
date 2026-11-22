import System;

using System;
using System.Collections;
using System.Linq;
using System.Net.Http;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        try
        {
            string url = @"http://www.baidu.com";

            HttpClient client = new HttpClient();
            client.DefaultRequestHeaders.Add("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3");
            HttpResponseMessage message = await client.GetAsync(url);
            HttpContent content = message.Content;
            string body = await content.ReadAsStringAsync();

            Console.WriteLine(body);
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.Message);
        }
    }

    private static void PrintArray(IEnumerable<int> arr)
    {
        foreach (int i in arr)
        {
            Console.WriteLine(i);
        }
    }
}

