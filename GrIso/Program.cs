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
            GraphTests.TestGraphIso(313, 313, 5000, 200*5000 / 2, false); Console.Read();  return;

            RandQuick.Shared.last = 2806392309;
            new GraphTests().TestGraphIso(6, 6);
            GraphTests.TestOk();
        }
    }
}
