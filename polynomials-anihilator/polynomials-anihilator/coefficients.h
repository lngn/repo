#pragma once
#include <iostream>

typedef unsigned int uint;

class coefficient139
{
	const static int mod = 139;
	int value = 0;
public:
	operator int() const { return this->value > mod/2 ? this->value - mod : this->value; }
	coefficient139(const coefficient139& arg) { this->value = arg.value; }
	coefficient139(int value) { this->value = 0 <= value ? value % mod : mod + value % mod; }
	coefficient139& operator=(const coefficient139& arg) { this->value = arg.value; return *this; }
	coefficient139& operator=(int value) { this->value = 0 <= value ? value % mod : mod + value % mod; return *this; }

	coefficient139 operator*(const coefficient139& arg) const { return value * arg.value; };
	coefficient139& operator*=(const coefficient139& arg) { *this = value * arg.value; return *this; };
	coefficient139 operator+(const coefficient139& arg) const { return value + arg.value; };
	coefficient139& operator+=(const coefficient139& arg) { *this = value + arg.value; return *this; };
	coefficient139 operator-(const coefficient139& arg) const { return value - arg.value; };
	coefficient139& operator-=(const coefficient139& arg) { *this = value - arg.value; return *this; };
	coefficient139 operator-() const { return -value; };

	bool operator<(const coefficient139& arg) const { return value < arg.value; };
	bool operator<=(const coefficient139& arg) const { return value <= arg.value; };
	bool operator>(const coefficient139& arg) const { return value > arg.value; };
	bool operator>=(const coefficient139& arg) const { return value >= arg.value; };
	bool operator==(const coefficient139& arg) const { return value == arg.value; };	
	bool operator!=(const coefficient139& arg) const { return value != arg.value; };

	bool operator<(int arg) const { return *this < coefficient139(arg); };
	bool operator<=(int arg) const { return *this <= coefficient139(arg); };
	bool operator>(int arg) const { return *this > coefficient139(arg); };
	bool operator>=(int arg) const { return *this >= coefficient139(arg);};
	bool operator==(int arg) const { return *this == coefficient139(arg); };
	bool operator!=(int arg) const { return *this != coefficient139(arg); };
};

/**
namespace std 
{
	std::ostream& operator<<(std::ostream& stream, coefficient139 coefficient)
	{
		stream << coefficient;
		return stream;
	}
}
/**/