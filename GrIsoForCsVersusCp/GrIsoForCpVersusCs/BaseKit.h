
#pragma once

const double RandQuick1Max = 1.0/pow(2.0,32.0);
struct RandQuick
{
	unsigned _last;	
	RandQuick(unsigned last = 1313) { _last = last; };
	unsigned Next() { _last=1664525*_last+ 1013904223;	return _last; };	
	RandQuick& operator()(void) { Next();return *this; };

	operator unsigned() { return (unsigned)_last; };
	operator int() { return (int)_last; };
	operator float() { return float(_last*RandQuick1Max); };
	operator double() { return double(_last*RandQuick1Max); };

	unsigned operator()(unsigned max) { return unsigned(RandQuick1Max*Next()*(1+max)); }
	unsigned operator()(unsigned min,unsigned max) { return unsigned(RandQuick1Max*Next()*(1+max-min)+min); }
	int operator()(int min,int max) { return int(RandQuick1Max*Next()*(1+max-min)+min); };
	float operator()(float min,float max) { return float(RandQuick1Max*Next()*(max-min)+min);};
	double operator()(double min,double max) { return double(RandQuick1Max*Next()*(max-min)+min); };
};

