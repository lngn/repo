
#pragma once
#include "math.h"

typedef unsigned int uint;
const uint rand_quick_seed = 1313;
const double rand_quick_1max = 1.0 / pow(2.0, 32.0);

class rand_quick
{
public:
	uint last;

	rand_quick(uint last = rand_quick_seed) { this->last = last; }

    static uint next(uint last) { return 1664525 * last + 1013904223; }
    uint next() { return last = next(last); }

    //uint next() { last = 1664525 * last + 1013904223; return last; }

    int operator()(int max) { double n = rand_quick_1max * next() * (1 + max); return (int)n; }
    int operator()(int min, int max) { double n = rand_quick_1max * next() * (max - min + 1) + min; return (int)n; }
};

