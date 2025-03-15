#pragma once


// We generally use modular prime field. 
// Using modular field has low impact for anihilator calculation but that rid us of burden coefficient overflow which happanes quickly even for very small coefficient (lot of exponentiation).
template<int mod>
class coefficient_mod
{
	const static int mod = mod;
	int value = 0;
public:
	bool overflow() const { return false; }
	operator int() const { return this->value > mod/2 ? this->value - mod : this->value; }
	coefficient_mod(const coefficient_mod& arg) { this->value = arg.value; }
	coefficient_mod(int value) { this->value = 0 <= value ? value % mod : mod + value % mod; }
	coefficient_mod& operator=(const coefficient_mod& arg) { this->value = arg.value; return *this; }
	coefficient_mod& operator=(int value) { this->value = 0 <= value ? value % mod : mod + value % mod; return *this; }

	coefficient_mod operator*(const coefficient_mod& arg) const { return value * arg.value; };
	coefficient_mod& operator*=(const coefficient_mod& arg) { *this = value * arg.value; return *this; };
	coefficient_mod operator+(const coefficient_mod& arg) const { return value + arg.value; };
	coefficient_mod& operator+=(const coefficient_mod& arg) { *this = value + arg.value; return *this; };
	coefficient_mod operator-(const coefficient_mod& arg) const { return value - arg.value; };
	coefficient_mod& operator-=(const coefficient_mod& arg) { *this = value - arg.value; return *this; };
	coefficient_mod operator-() const { return -value; };

	bool operator<(const coefficient_mod& arg) const { return value < arg.value; };
	bool operator<=(const coefficient_mod& arg) const { return value <= arg.value; };
	bool operator>(const coefficient_mod& arg) const { return value > arg.value; };
	bool operator>=(const coefficient_mod& arg) const { return value >= arg.value; };
	bool operator==(const coefficient_mod& arg) const { return value == arg.value; };	
	bool operator!=(const coefficient_mod& arg) const { return value != arg.value; };

	bool operator<(int arg) const { return *this < coefficient_mod(arg); };
	bool operator<=(int arg) const { return *this <= coefficient_mod(arg); };
	bool operator>(int arg) const { return *this > coefficient_mod(arg); };
	bool operator>=(int arg) const { return *this >= coefficient_mod(arg);};
	bool operator==(int arg) const { return *this == coefficient_mod(arg); };
	bool operator!=(int arg) const { return *this != coefficient_mod(arg); };
};

// We generally use modular prime field. 
// This is for small calculation and demonstration and this uses overflow marker INT_MIN.
class coefficient_int
{	
	int value = 0;
public:
	bool overflow() const { return value == INT_MIN;  }
	static bool overflow(int value) { return value == INT_MIN;  };
	static bool overflow(int64_t value) { 
		if (value < INT_MIN || value > INT_MAX)
			return true;
		else
			return false;
		//return value <= INT_MIN || value >= INT_MAX;  
	};

	operator int() const { return this->value; }
	coefficient_int(const coefficient_int& arg) { this->value = arg.value; }
	coefficient_int(int value) { 
		this->value = value; 
	}
	coefficient_int(int64_t value) { this->value = overflow(value) ? INT_MIN : int(value); }
	coefficient_int& operator=(const coefficient_int& arg) { this->value = arg.value; return *this; }
	coefficient_int& operator=(int value) { 
		this->value = value; return *this; 
	}
	coefficient_int& operator=(int64_t value) { this->value = overflow(value) ? INT_MIN : int(value); return *this; }

	coefficient_int operator*(const coefficient_int& arg) const { return overflow() || arg.overflow() ? INT_MIN : int64_t(value)* arg.value;  };
	coefficient_int& operator*=(const coefficient_int& arg) { *this = overflow() || arg.overflow() ? INT_MIN : int64_t(value) * arg.value; return *this; };
	coefficient_int operator+(const coefficient_int& arg) const { return overflow() || arg.overflow() ? INT_MIN : int64_t(value) + arg.value; };
	coefficient_int& operator+=(const coefficient_int& arg) { *this = overflow() || arg.overflow() ? INT_MIN : int64_t(value) + arg.value; return *this; };
	coefficient_int operator-(const coefficient_int& arg) const { return overflow() || arg.overflow() ? INT_MIN : int64_t(value) - arg.value; };
	coefficient_int& operator-=(const coefficient_int& arg) { *this = overflow() || arg.overflow() ? INT_MIN : int64_t(value) - arg.value; return *this; };
	coefficient_int operator-() const { return overflow() ? INT_MIN: -value; };

	bool operator<(const coefficient_int& arg) const { return value < arg.value; };
	bool operator<=(const coefficient_int& arg) const { return value <= arg.value; };
	bool operator>(const coefficient_int& arg) const { return value > arg.value; };
	bool operator>=(const coefficient_int& arg) const { return value >= arg.value; };
	bool operator==(const coefficient_int& arg) const { return value == arg.value; };
	bool operator!=(const coefficient_int& arg) const { return value != arg.value; };

	bool operator<(int arg) const { return value < arg; };
	bool operator<=(int arg) const { return value <= arg; };
	bool operator>(int arg) const { return value > arg; };
	bool operator>=(int arg) const { return value >= arg; };
	bool operator==(int arg) const { return value == arg; };
	bool operator!=(int arg) const { return value != arg; };
};