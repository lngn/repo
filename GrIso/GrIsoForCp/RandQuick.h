
#pragma once
#include "math.h"

namespace GrIso
{
	typedef unsigned int uint;
	typedef unsigned short ushort;
	const uint RandQuickSeed = 1313;
	const double RandQuick1Max = 1.0 / pow(2.0, 32.0);
	
    class RandQuick
    {
	public:
		uint last;

        RandQuick(uint last = RandQuickSeed) { this->last = last; }
        uint Next() { last = 1664525 * last + 1013904223; return last; }
        int Next(int max) { double n = RandQuick1Max * Next() * (1 + max); return (int)n; }
        int Next(int min, int max) { double n = RandQuick1Max * Next() * (max - min + 1) + min; return (int)n; }
	};

	RandQuick RandQuickShared;
}