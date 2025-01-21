
#include <tuple>
#include <cstdarg>
#include "polynomial.h"

void assert(bool cond, const char* message, ...)
{
	if (!cond)
	{
		va_list va_args;
		va_start(va_args, message);
		vprintf(message, va_args);
		printf("\r\n");
	}
}

void test_parse(const std::string& istring, const std::string& ostring)
{
	polynomial<int, int> polynomial;
	bool res = polynomial.parse(istring);
	assert(res, "test_parse failed %s", istring.c_str());
	std::string pstring = polynomial.string();
	assert(polynomial.normalized(true),"test_parse not normalized %s", istring.c_str());
	assert(pstring == (ostring.empty() ? istring : ostring), "test_parse not match %s", istring.c_str());
}

void test_parse(const std::vector<std::tuple<std::string, std::string>>& strings)
{
	for (const auto& string : strings)	
		test_parse(std::get<0>(string), std::get<1>(string));
}

void test_parse()
{
	test_parse({ { "0", "0" }, { " 0 ", "0" }, { "-1" ,"-1" }, { "-1    " ,"-1" }, { "1", "1"},  { "  1", "1" },{ "43 ", "43" },{ " -201", "-201" } });
	test_parse({ {"x-10",""}, {"-y-11", ""}, {"10*x+5",""}, {"-11*y+5*x",""},{"11*y-7*x+21",""},{"11*y-7*x-4*b+3*a-11",""} });
	test_parse({ { " -10 +x", "x-10"}, {"-y -11 +3 -2y", "-3*y-8"}, {" 10x + 5 ", "10*x+5"}, {"5*y+2x -3*x +6x + 15y -9*y", "11*y+5*x"} });
}

void test_arithmetic(const std::string& result, const polynomial<int, int>& value)
{
	assert(value.normalized(true), "parsed not normalized %s", result.c_str());
	const std::string s = value.string();
	assert(s == result, "test_arithmetic failed %s", result.c_str());
}

void test_arithmetic(const std::vector<std::tuple<std::string, polynomial<int, int>>> & values)
{
	for (const auto& value : values)
		test_arithmetic(std::get<0>(value), std::get<1>(value));
}

void test_arithmetic()
{
	const polynomial<int, int> x('x'), y('y');
	polynomial<int, int> r;
	std::string s;

	test_arithmetic({ {"x+1", x + 1}, {"-2*x+3", -2*x + 3}, {"x+5", 5+x},{"-3*x-3", -3-3*x} });
	/*
	r = x + x;
	s = r.string();
	assert(s == "2*x");

	r = x + x;
	s = r.string();
	assert(s == "2*x");

	r = -x - 2;
	s = r.string();
	assert(s == "-x-2");

	r = 3 + x;
	s = r.string();
	assert(s == "x+3");

	r = x*x;
	s = r.string();
	assert(s == "x2");

	r = x ^ 3;
	s = r.string();
	assert(s == "x3");

	r = (x ^ 2) + (x + 4);
	s = r.string();
	assert(s == "x2+x+4");

	r = ((x ^ 2) + (x + 4))*(x+2);
	s = r.string();
	assert(s == "x3+3*x2+6*x+8");
	*/
}

void test_polynomials()
{

}