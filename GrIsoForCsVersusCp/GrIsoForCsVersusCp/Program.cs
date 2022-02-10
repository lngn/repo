using System;
using GrIso;

namespace GrIsoForCs
{
    class Program
    {
        static void Main(string[] args)
        {
            OkTests.TestGraphIso(313, 313, 5000, 200*5000 / 2, true); Console.Read();  return;

            //OkTests.TestOk();
        }
    }
}
