using System;


namespace GrIso
{
    class AbortException : Exception
    {
        public AbortException(string message) : base(message) { }
    }


    class Program
    {
        public static void Abort(string message) 
        {
            throw new AbortException(message); 
        }

        static void Main(string[] args)
        {
            //GraphHarderIsoTests.RunRandomIso(); return;

            //GraphHarderIsoTests.RunHarderIso();return;

            GraphHarderIsoTests.TestHarderIso(); return;

            //GraphTests.TestGraphIso(313, 313, 5000, 200*5000 / 2, false); Console.Read();  return;

            //GraphTests.TestOk();
        }
    }
}
