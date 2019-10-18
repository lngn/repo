using System;
using GrIso;

namespace ShortTestsForCs
{
    class Program
    {
        static void Main(string[] args)
        {
            //new OkTests().TestOneShortListCorrectness(32, 992, 2883102456, 548308047);

            new OkTests(559).TestAllShortListCorrectness(39);
            //new OkTests(5589).TestShortListPerformace(39);
            Console.WriteLine("Done");
            Console.ReadKey();
        }
    }
}
