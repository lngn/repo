
using System;

namespace GrIso
{
    class RandQuick
    {
        const uint RandQuickSeed = 1313;

        public static double RandQuick1Max = 1.0 / Math.Pow(2.0, 32.0);
        public uint last;

        public RandQuick(uint last = RandQuickSeed) { this.last = last; }
        public uint RawNext() { last = 1664525 * last + 1013904223; return last; }
        public int Next(int max) { double n = RandQuick1Max * RawNext() * (1 + max); return (int)n; }
        public int Next(int min, int max) { double n = RandQuick1Max * RawNext() * (max - min + 1) + min; return (int)n; }
        public double Next(double max) { double n = RandQuick1Max * RawNext() *  max; return n; }
        public double Next(double min, double max) { double n = RandQuick1Max * RawNext() * (max - min)+ min; return n; }

        public static RandQuick Shared = new RandQuick();
    }

    struct HashQuick
    {
        public uint last;
        public int Hash(uint value) { last = (1664525 * last + 1013904223) ^ value; return (int)last; }
        public int Hash(int value) { last = (1664525 * last + 1013904223) ^ (uint)value; return (int)last; }
        public int Hash() { return (int)last; }
    }
}