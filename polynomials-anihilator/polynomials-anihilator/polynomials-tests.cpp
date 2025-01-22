
#include <tuple>
#include <cstdarg>
#include <random>
#include "rand_quick.h"
#include "polynomial.h"

rand_quick rand_quick;

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
	assert(polynomial.normalized(),"test_parse not normalized %s", istring.c_str());
	assert(pstring == (ostring.empty() ? istring : ostring), "test_parse not match %s", istring.c_str());
}

void test_parse()
{
	test_parse("0", "0");
	test_parse(" 0 ", "0");
	test_parse("-1", "-1");
	test_parse("-1    ", "-1");
	test_parse("1", "1");
	test_parse("  1", "1");
	test_parse("43 ", "43");
	test_parse(" -201", "-201");

	test_parse("x-10","");
	test_parse("-y-11", "");
	test_parse("10*x+5", "");
	test_parse("-11*y+5*x", "");
	test_parse("11*y-7*x+21", "");
	test_parse("11*y-7*x-4*b+3*a-11", "");

	test_parse(" -10 +x", "x-10");
	test_parse("-y -11 +3 -2y", "-3*y-8");
	test_parse(" 10x + 5 ", "10*x+5");
	test_parse("5*y+2x -3*x +6x + 15y -9*y", "11*y+5*x");

	test_parse("3*x3y2+ x2+ y2 - 3x3y2 -y2 -x2", "0");
	test_parse("3*x3y2+ x2+ y2 - 2x3y2 -2y2 -x2", "y2*x3-y2");
	test_parse("z2x2+2z3x2*y-2*x2*yz3-z2y2","-z2*y2+z2*x2");
}

void test_arithmetic(const std::string& result, const polynomial<int, int>& value)
{
	assert(value.normalized(), "parsed not normalized %s", result.c_str());
	const std::string s = value.string();
	assert(s == result, "test_arithmetic failed %s", result.c_str());
}

void test_compose()
{
	polynomial<int, int> x('x'), y('y'), n(3);
	std::vector<char> variables = { 'x' };

	polynomial<int, int> w1, w2, w3, w4, w5, w6, w7;
	w1 = (x + 1) * (x + 2);
	w2 = (x ^ 2) + 3;

	w3 = w1(variables, { w2 });
	w4 = w2(variables, { n });
	w5 = w1(variables, { w4 });
	w6 = w3(variables, { n });

	assert(w5 == w6, "!!!!");

	std::mt19937 gen(1313);
	std::uniform_int_distribution<unsigned> dist(0,5);
	int v1 = dist(gen);
	int v2 = dist(gen);
	int v3 = dist(gen);

	int v4 = rand_quick(0, 1);
	int v5 = rand_quick(0, 1);
	int v6 = rand_quick(0, 1);
}

void test_arithmetic(int seed, int variables_max, int terms_max, int total_max, int coefficient_max)
{
	int variables_count = rand_quick(variables_max);
	int terms_count = rand_quick(terms_max);
}

void test_arithmetic()
{
	const polynomial<int, int> x('x'), y('y');
	polynomial<int, int> r;
	std::string s;

	test_arithmetic("x+1", x + 1);
	test_arithmetic("-2*x+3", -2 * x + 3);
	test_arithmetic("x+5", 5 + x);
	test_arithmetic("-3*x-3", -3 - 3 * x);
	test_arithmetic("x", 2 + x + 3 - 5);
	test_arithmetic("0", 2 + x - 2 - x);
	test_arithmetic("2", 2 * x + 1 - x + 1 - x);

	test_arithmetic("2*x", x + x);
	test_arithmetic("x2", x * x);
	test_arithmetic("0", x + x - 2 * x);
	test_arithmetic("x2+3*x+2", (x + 1) * (x + 2));
	test_arithmetic("x2", x ^ 2);
	test_arithmetic("x5", x ^ 5);
	test_arithmetic("x3+3*x2+3*x+1", (x + 1) ^ 3);
	test_arithmetic("y2+3*y*x+x2+y+x+1", ((x+y)^2) + (x+1)*(y+1));
}



void test_polynomials()
{

}