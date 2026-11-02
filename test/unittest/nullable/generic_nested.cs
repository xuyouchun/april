// EXPECT_WARNING 4006 4006 4006 4004 4006 4006 4006

/* EXPECT

*/

#nullable enable
import System;
using System;
using System.Collections.Generic;
class Program {
  static List<String?> Make() { return new List<String?>(); }
  static void Take(String s) { }
  static void Cases() {
    // method return type with arguments
    Console.WriteLine(Make()[0].Length);               // 1 warn
    // nested arguments
    Dictionary<String, List<String?>> d = new Dictionary<String, List<String?>>();
    Console.WriteLine(d["k"][0].Length);                // 2 warn (inner list maybe)
    Dictionary<String, List<String>> e = new Dictionary<String, List<String>>();
    Console.WriteLine(e["k"][0].Length);                // ok
    // arrays
    String?[] arr = new String?[2];
    Console.WriteLine(arr[0].Length);                   // 3 warn
    String[] arr2 = new String[2];
    Console.WriteLine(arr2[0].Length);                  // ok
    // foreach
    List<String?> l = new List<String?>();
    foreach (String s in l) { Console.WriteLine(s.Length); }   // 4 warn (conversion in foreach)
    foreach (String? s2 in l) { Console.WriteLine(s2.Length); } // 5 warn
    List<String> l2 = new List<String>();
    foreach (String s3 in l2) { Console.WriteLine(s3.Length); } // ok
    foreach (var s4 in l) { Console.WriteLine(s4.Length); }     // 6 warn
    foreach (String? s5 in arr) { Console.WriteLine(s5.Length); } // 7 warn
    foreach (String s6 in arr2) { Console.WriteLine(s6.Length); } // ok
  }
  [EntryPoint] public static void Main() { }
}
