using System;
using GrIso;

namespace GrIsoForCs
{
    class Program
    {
        static void Main(string[] args)
        {
            OkTests.TestGraphIso(333, 100, 1000, true); Console.Read();  return;

            OkTests.TestOk();
        }
    }
}
