
#include <tuple>
#include <cassert>
#include "polynomial.h"

const polynomial<int, int> x, y, z;

void test_parse(const std::string & istring)
{
	polynomial<int, int> polynomial;	
	polynomial.parse(istring);
	std::string pstring = polynomial.string();
	assert(polynomial.normalized(true));
	assert(pstring == istring);
}

void test_parse2(const std::string& istring, const std::string& ostring)
{
	polynomial<int, int> polynomial;
	polynomial.parse(istring);
	std::string pstring = polynomial.string();
	assert(polynomial.normalized(true));	
	assert(pstring == ostring);
}

void test_parse(const std::vector<std::string> & strings)
{	
	for (const auto& string : strings)
		test_parse(string);
}

void test_parse2(const std::vector<std::tuple<std::string, std::string>>& strings)
{
	for (const auto& string : strings)	
		test_parse2(std::get<0>(string), std::get<1>(string));
}

void test_parse()
{
	test_parse2("5*y+2x -3*x +6x + 15y -9*y", "11*y+5*x");

	test_parse("y2-x");

	test_parse("-10*y*x2");

	test_parse2("-10x2", "-10*x2");

	test_parse2("-10x2*y3", "-10*y3*x2");

	test_parse2("-10x2*y3-11*y3x2", "-21*y3*x2");

	test_parse2({ {" 0 ", "0"} });

	test_parse({ "0", "-1", "1", "-201", "43" });
	test_parse2({{ " 0 ", "0" }, { "-1 " ,"-1" }, { "  1", "1" }});
	test_parse({ "x-10", "-y-11", "10*x+5", "-11*y+5*x","11*y-7*x+21"});
	test_parse2({ { " -10 +x", "x-10"}, {"-y -11 +3 -2y", "-3*y-8"}, {" 10x + 5 ", "10*x+5"}, {"5*y+2x -3*x +6x + 15y -9*y", "11*y+5*x"} });
}


void test_arithmetic()
{
	const polynomial<int, int> x(polynomial_coefficient<int, int>(1, polynomial_exponent<int>('x',1 )));//, y, z;
	polynomial<int, int> r;
	std::string s;

	r = x + x;
	//r = x ^ 2;
	s = r.string();
	assert(s == "2*x");
}

void test_polynomials()
{

}