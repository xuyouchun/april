/* EXPECT_ERROR does not implement interface member */

import System;

using System;

interface IShape
{
    void Draw();
    int Area(int scale);
}

class Square : IShape
{
    public void Draw()
    {
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
    }
}
