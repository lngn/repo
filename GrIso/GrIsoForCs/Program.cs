using System;
using GrIso;

namespace GrIsoForCs
{
    class Program
    {
        static void Main(string[] args)
        {
            var tests = new OkTests(1919);
            //tests.TestAllShortListCorrectness(39);
            //tests.TestGraphCompare();
            tests.TestGraphIso();
            
            Console.WriteLine("Done");
            Console.ReadKey();
            return;

            //new OkTests().PrintRandGraphPermutation(10, 16);
            //new OkTests().PrintRandGraphPermutation(16, 32);
            //new OkTests().TestGraphCompare();
            new OkTests().TestGraphIso();
            //new OkTests().TestOneShortListCorrectness(32, 992, 2883102456, 548308047);

            //new OkTests(559).TestAllShortListCorrectness(39);
            //new OkTests(5589).TestShortListPerformace(39);
            Console.WriteLine("Done");
            Console.ReadKey();
        }
    }
}
