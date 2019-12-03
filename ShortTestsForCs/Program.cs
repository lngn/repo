using System;
using GrIso;

namespace ShortTestsForCs
{
    class Program
    {
        static void Main(string[] args)
        {
            var tests = new OkTests(1919);


            tests.TestOneVeryShortListCorrectness(2,10,876265113,2891641121);
            tests.TestAllVeryShortListCorrectness(39);

            //tests.TestGraphIso(1000, 1000 * 20 / 2);
            tests.TestOneShortListCorrectness(128, 1024);

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
