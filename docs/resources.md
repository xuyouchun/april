# Resources

A project can embed files in the assembly that it makes, and the program reads them by a name
(`System.Runtime.Resource`). The files need not be there when the program runs (the interpreter, the JIT and the
executables of `april compile --native` all have them).

## In the project

```xml
<project>
    <name>App</name>
    <files> <file><path>*.cs</path></file> </files>

    <resources>
        <resource><path>data/*.bin</path></resource>
        <resource><path>hello.txt</path><name>App.Greeting</name></resource>
    </resources>
</project>
```

* `path` is relative to the directory of the project; a wildcard takes the files that match it, a directory takes all
  its files.
* The name of a resource is the name of the project and the path of the file from the directory of the project, with `.`
  in place of `/`: `data/numbers.bin` of the project `App` is `App.data.numbers.bin`. `<name>` gives another name, for a
  path that is one file.
* A file that cannot be read is an error of the compiler (AP0903). A change of a file is a new build of `april run`
  (the files are in the key of the cache).

## In the program

```csharp
using System.Runtime;

Boolean has = Resource.Exists("App.Greeting");
Int32 size = Resource.GetSize("App.data.numbers.bin");      // ArgumentException when there is none
UInt8[] bytes = Resource.GetBytes("App.data.numbers.bin");  // a new array: the copy is the program's
ReadOnlySpan<UInt8> view = Resource.GetSpan("App.data.numbers.bin");
```

The assemblies that are loaded are searched by the name, the first that has it gives the bytes. A name should begin with
the name of the project, so that two assemblies do not have the same one.

## How it is made

* The assembly has a table `resource` (name, and the bytes as a block of the heap): the layout version 1 has it.
* The runtime library has two functions (`Resource_GetSize`, `Resource_Read`); `Resource` makes the array of the size and
  has the function fill it. The array is a copy: there is no `Span` on the data of an assembly (a `Span` is on an
  array).
* The native programs have no assemblies: a program that reads resources (one of the two functions is reachable) has the
  bytes of the resources of all its assemblies as constants of the module, and registers them at the start
  (`april_native_add_resource`).
