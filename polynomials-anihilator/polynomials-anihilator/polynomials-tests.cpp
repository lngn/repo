
#include <tuple>
#include <cstdarg>
#include <vector>
#include "rand_quick.h"
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

void test_compose01()
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
}

void test_compose(uint seed, int variables_max, int terms_max, int total_max, int coefficient_max)
{
	rand_quick rand_quick(seed);
	int variables_count = rand_quick(1, variables_max);
	int terms_count = rand_quick(1, terms_max);
	auto generate = [=, &rand_quick]()->polynomial<int, int>
	{
		polynomial<int, int> result;
		for (int c = terms_count;c > 0;--c)
		{
			polynomial_coefficient<int, int> term = rand_quick(-coefficient_max, coefficient_max);
			int total = rand_quick(1, total_max);
			while (total >0 )
			{
				int exponent = rand_quick(1, total);
				char variable = char('a' + rand_quick(0, variables_count-1));
				term.exponents.push_back(polynomial_exponent<int>(variable,exponent));
				total -= exponent;
			}
			result.push_back(term);
		}
		result.normalize(true);
		return result;
	};

	std::vector<char> variables(variables_count);
	for (char i = 0;i < variables_count;++i)
		variables[i] = 'a' + i;
	polynomial<int, int> compose_function = generate();

	std::vector<polynomial<int, int>> argsV;
	std::vector<std::reference_wrapper<polynomial<int, int>>> argsR;
	for (int i = 0;i < variables_count;++i)
		argsV.emplace_back(generate());
	for (int i = 0;i < variables_count;++i)	
		argsR.emplace_back(argsV[i]);
	for (int i = 0;i < variables_count;++i)
		assert(argsV[i] == argsR[i], "reference_wrapper differ");
	

	std::vector<polynomial<int, int>> numsV;
	std::vector<std::reference_wrapper<polynomial<int, int>>> numsR;
	for (int i = 0;i < variables_count;++i)
		numsV.emplace_back(rand_quick(-coefficient_max, coefficient_max));
	for (int i = 0;i < variables_count;++i)
		numsR.emplace_back(numsV[i]);
	for (int i = 0;i < variables_count;++i)
		assert(numsV[i] == numsR[i], "reference_wrapper differ");

	std::vector<polynomial<int, int>> evalsV;
	std::vector<std::reference_wrapper<polynomial<int, int>>> evalsR;
	for (int i = 0;i < variables_count;++i)
		evalsV.emplace_back(argsV[i](variables, numsR));
	for (int i = 0;i < variables_count;++i)
		evalsR.emplace_back(evalsV[i]);
	for (int i = 0;i < variables_count;++i)
		assert(evalsV[i] == evalsR[i], "reference_wrapper differ");

	polynomial<int, int> compose_result = compose_function(variables, argsR);
	polynomial<int, int> compose_evaluated = compose_result(variables, numsR);
	polynomial<int, int> function_evaluated = compose_function(variables, evalsR);
	if (false)
	{
		std::cout << "compose_function: " << compose_function.string() << std::endl;
		std::cout << "arguments: " << std::endl;
		for (int i = 0;i < variables_count;++i)
			std::cout << "variable " << variables[i] <<": " << argsV[i].string() << std::endl;
		std::cout << "compose_result: " << compose_result.string() << std::endl;
		std::cout << "numbers: " << std::endl;
		for (int i = 0;i < variables_count;++i)
			std::cout << "variable " << variables[i] << ": " << numsV[i].string() << " " << std::endl;
		std::cout << "evaluated arguments: " << std::endl;
		for (int i = 0;i < variables_count;++i)
			std::cout << "variable " << variables[i] << ": " << evalsV[i].string() << std::endl;
	}
	assert(function_evaluated == compose_evaluated,
		"test_compose failed seed=%u, variables_max=%d, terms_max=%d, total_max=%d, coefficient_max=%d", 
		seed, variables_max, terms_max, total_max, coefficient_max
	);
}

void test_compose02()
{
	test_compose(1316, 3, 10, 10, 100);
	uint seed = rand_quick_seed;
	for (int c = 1000;c > 0;--c)
	{
		test_compose(++seed, 3, 10, 10, 100);
	}
}

void test_compose()
{
	//test_compose(1375, 2, 2, 2, 5);
	//test_compose(1375, 2, 10, 10, 5);
	test_compose(1316, 3, 10, 10, 100);
	uint seed = rand_quick_seed;
	for (int c = 1000;c > 0;--c)
	{
		test_compose(++seed, 3, 5, 5, 5);
	}
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



int count_exponents(int variables_count, int total_max)
{
	if (variables_count == 1)
		return total_max + 1;
	int count = 0;
	for (int i = 0;i <= total_max;++i)
		count += count_exponents(variables_count - 1, total_max - i);
	return count;
}

void test_exponents(int variables_count, int total_max)
{
	std::vector<polynomial<int, int>> testV;
	polynomial<int, int> testP = 1;
	for (char variable = 'a' + variables_count;variable > 'a'; --variable)
		testP.front().exponents.emplace_back(variable, 0);
	for (;testP.front().total() <= total_max; ++testP.front().exponents)
		testV.push_back(testP);
	assert(testV.size() == count_exponents(variables_count, total_max),"test_exponents incorrect operator ++ or incorrect count of different exponents");

	for (auto it=testV.begin(),itE=testV.end();it != itE;++it)
	{
		testP = *it;
		assert(testP == *it, "test_exponents incorrect operator ==");
		assert(testP <= *it, "test_exponents incorrect operator <=");
		++testP.front().exponents;
		--testP.front().exponents;
		assert(testP == *it, "test_exponents incorrect operator --");		
		for (auto itN = it+1; itN != itE;++itN)
		{
			assert(testP < *itN, "test_exponents incorrect operator <");
			assert(testP <= *itN, "test_exponents incorrect operator <=");
			assert(testP != *itN, "test_exponents incorrect operator !=");
		}
	}
}
void test_exponents()
{
	//test_exponents(3,3);
	for (int variables_count = 1;variables_count <= 5; ++variables_count)
		for (int total_max = 1;total_max <= 5; ++total_max)
			test_exponents(variables_count, total_max);
}

void test_exponents_increment()
{
	test_exponents();
}

/**
void test_exponents_increment()
{
	int c1 = test01(1, 4);
	int c2 = test01(2, 4);
	int c3 = test01(3, 4);
	int c6 = test01(4, 5);


	polynomial<int,int> test = 1, test_dec = 1;
	test.front().exponents.emplace_back('d', 0);
	test.front().exponents.emplace_back('c', 0);
	test.front().exponents.emplace_back('b', 0);
	test.front().exponents.emplace_back('a', 0);
	
	
	int c4 = 0;
	test.normalize(true);
	std::vector<std::string> test01;
	while (test.front().total() <= 5) 
	{
		++c4;
		test01.emplace_back(test.string()); 
		std::cout << test01.back() <<std::endl;
	
	

		++test.front().exponents; 

		/**
		test_dec = test;
		++test_dec.front().exponents;
		--test_dec.front().exponents;
		if (test_dec != test)
			assert(test == test_dec, "zzzzzz");
		/**
	}
	assert(c4 == c6, "zzzzzzz");

	std::vector<std::string> test02;
}
/**/

void test_anihilate()
{
	
}

void test_polynomials()
{

}