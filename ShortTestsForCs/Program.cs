using System;
using GrIso;

namespace ShortTestsForCs
{
    class Program
    {
        static void Main(string[] args)
        {
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
