
using System;

namespace GrIso
{
    class RandQuick
    {
        public static double RandQuick1Max = 1.0 / Math.Pow(2.0, 32.0);
        public uint last;

        public RandQuick(uint last = 1313) { this.last = last; }
        public uint Next() { last = 1664525 * last + 1013904223; return last; }
        public int Next(int max) { double n = RandQuick1Max * Next() * (1 + max); return (int)n; }
        public int Next(int min, int max) { double n = RandQuick1Max * Next() * (max - min + 1) + min; return (int)n; }

        public static RandQuick Shared = new RandQuick();
    }
}