
#include <tuple>
#include <cstdarg>
#include <ctime>
#include <vector>
#include "rand_quick.h"
#include "coefficients.h"
#include "polynomial.h"

using coefficient139 = coefficient_mod<139>;
using polynomial139 = polynomial<int, coefficient139>;
using polynomial_coefficient139 = polynomial_coefficient<int, coefficient139>;
using polynomials_anihilator139 = polynomials_anihilator<int, coefficient139>;

//typedef polynomial139 polynomial_test;
//typedef polynomial_coefficient139 coefficient_test;
//typedef polynomials_anihilator139 anihilator_test;

//typedef polynomial<int, int> polynomial_test;
//typedef polynomial_coefficient<int, int> coefficient_test;
//typedef polynomials_anihilator<int, int> anihilator_test;

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

template<class coefficient_number>
void test_parse(const std::string& istring, const std::string& ostring)
{
	using polynomial_test = polynomial<int, coefficient_number>;
	
	polynomial_test polynomial;
	bool res = polynomial.parse(istring);
	assert(res, "test_parse failed %s", istring.c_str());
	std::string pstring = polynomial.string();
	assert(polynomial.normalized(),"test_parse not normalized %s", istring.c_str());
	assert(pstring == (ostring.empty() ? istring : ostring), "test_parse not match %s", istring.c_str());
}

template<class coefficient_number>
void test_parse()
{
	std::cout << "test_parse";
	
	test_parse<coefficient_number>("0", "0");
	test_parse<coefficient_number>(" 0 ", "0");
	test_parse<coefficient_number>("-1", "-1");
	test_parse<coefficient_number>("-1    ", "-1");
	test_parse<coefficient_number>("1", "1");
	test_parse<coefficient_number>("  1", "1");
	test_parse<coefficient_number>("43 ", "43");
	test_parse<coefficient_number>(" -29", "-29");

	test_parse<coefficient_number>("x-10","");
	test_parse<coefficient_number>("-y-11", "");
	test_parse<coefficient_number>("10*x+5", "");
	test_parse<coefficient_number>("-11*y+5*x", "");
	test_parse<coefficient_number>("11*y-7*x+21", "");
	test_parse<coefficient_number>("11*y-7*x-4*b+3*a-11", "");

	test_parse<coefficient_number>(" -10 +x", "x-10");
	test_parse<coefficient_number>("-y -11 +3 -2y", "-3*y-8");
	test_parse<coefficient_number>(" 10x + 5 ", "10*x+5");
	test_parse<coefficient_number>("5*y+2x -3*x +6x + 15y -9*y", "11*y+5*x");

	test_parse<coefficient_number>("3*x3y2+ x2+ y2 - 3x3y2 -y2 -x2", "0");
	test_parse<coefficient_number>("3*x3y2+ x2+ y2 - 2x3y2 -2y2 -x2", "y2*x3-y2");
	test_parse<coefficient_number>("z2x2+2z3x2*y-2*x2*yz3-z2y2","-z2*y2+z2*x2");

	std::cout << std::endl;
}

template<class coefficient_number>
void test_arithmetic(const std::string& result, const polynomial<int, coefficient_number>& value)
{
	assert(value.normalized(), "parsed not normalized %s", result.c_str());
	const std::string s = value.string();
	assert(s == result, "test_arithmetic failed %s", result.c_str());
}

template<class coefficient_number>
void test_arithmetic()
{
	using polynomial_test = polynomial<int, coefficient_number>;

	std::cout << "test_arithmetic";

	const polynomial_test x('x'), y('y');
	polynomial_test r;
	std::string s;

	test_arithmetic<coefficient_number>("x+1", x +  1);
	test_arithmetic<coefficient_number>("-2*x+3", -2 * x + 3);
	test_arithmetic<coefficient_number>("x+5", 5 + x);
	test_arithmetic<coefficient_number>("-3*x-3", -3 - 3 * x);
	test_arithmetic<coefficient_number>("x", 2 + x + 3 - 5);
	test_arithmetic<coefficient_number>("0", 2 + x - 2 - x);
	test_arithmetic<coefficient_number>("2", 2 * x + 1 - x + 1 - x);

	test_arithmetic<coefficient_number>("2*x", x + x);
	test_arithmetic<coefficient_number>("x2", x * x);
	test_arithmetic<coefficient_number>("0", x + x - 2 * x);
	test_arithmetic<coefficient_number>("x2+3*x+2", (x + 1) * (x + 2));
	test_arithmetic<coefficient_number>("x2", x ^ 2);
	test_arithmetic<coefficient_number>("x5", x ^ 5);
	test_arithmetic<coefficient_number>("x3+3*x2+3*x+1", (x + 1) ^ 3);
	test_arithmetic<coefficient_number>("y2+3*y*x+x2+y+x+1", ((x + y) ^ 2) + (x + 1) * (y + 1));

	std::cout << std::endl;
}

template<class coefficient_number>
polynomial<int, coefficient_number> random_polynomial(rand_quick& rand_quick, int terms_count, int variables_count, int total_max, int coefficient_max)
{
	polynomial<int, coefficient_number> result;
	for (int c = terms_count;c > 0;--c)
	{
		auto term = polynomial_coefficient<int, coefficient_number>(rand_quick(-coefficient_max, coefficient_max));
		int total = rand_quick(1, total_max);
		while (total > 0)
		{
			int exponent = rand_quick(1, total);
			char variable = char('a' + rand_quick(0, variables_count - 1));
			term.exponents.push_back(polynomial_exponent<int>(variable, exponent));
			total -= exponent;
		}
		result.push_back(term);
	}
	result.normalize(true);
	return result;
}

template<class coefficient_number>
void test_compose(uint seed, int variables_max, int terms_max, int total_max, int coefficient_max)
{
	using polynomial_test = polynomial<int, coefficient_number>;

	rand_quick rand_quick(seed);
	int variables_count = rand_quick(1, variables_max);
	int terms_count = rand_quick(1, terms_max);
	auto generate = [=, &rand_quick]()->polynomial_test
	{
		return random_polynomial<coefficient_number>(rand_quick, terms_count, variables_count, total_max, coefficient_max);
	};

	std::vector<char> variables(variables_count);
	for (char i = 0;i < variables_count;++i)
		variables[i] = 'a' + i;
	polynomial_test compose_function = generate();

	std::vector<polynomial_test> argsV;
	for (int i = 0;i < variables_count;++i)
		argsV.emplace_back(generate());

	std::vector<polynomial_test> numsV;
	for (int i = 0;i < variables_count;++i)
		numsV.emplace_back(rand_quick(-coefficient_max, coefficient_max));

	std::vector<polynomial_test> evalsV;
	for (int i = 0;i < variables_count;++i)
		evalsV.emplace_back(argsV[i](variables, numsV));
	

	polynomial_test compose_result = compose_function(variables, argsV);
	polynomial_test compose_evaluated = compose_result(variables, numsV);
	polynomial_test function_evaluated = compose_function(variables, evalsV);
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
	bool o1 = function_evaluated.overflow();
	bool o2 = compose_evaluated.overflow();
	assert(function_evaluated == compose_evaluated,
		"test_compose failed seed=%u, variables_max=%d, terms_max=%d, total_max=%d, coefficient_max=%d", 
		seed, variables_max, terms_max, total_max, coefficient_max
	);
}

template<class coefficient_number>
void test_applicator(uint seed, int variables_max, int terms_max, int total_max, int coefficient_max, int repeat_count)
{
	using polynomial_test = polynomial<int, coefficient_number>;
	using anihilator_test = polynomials_anihilator<int, coefficient_number>;

	rand_quick rand_quick(seed);
	int variables_count = rand_quick(1, variables_max);
	int terms_count = rand_quick(1, terms_max);
	auto generate = [=, &rand_quick]()->polynomial_test
	{
		return random_polynomial<coefficient_number>(rand_quick, terms_count, variables_count, total_max, coefficient_max);
	};

	anihilator_test anihilator;
	std::vector<char> variables(variables_count);
	for (char i = 0;i < variables_count;++i)
		anihilator.arg_variables.push_back('a' + i);

	std::vector<polynomial_test> argsV;
	for (int i = 0;i < variables_count;++i)
		anihilator.arg_polynomials.push_back(generate());

	anihilator.init();
	for (int c = 0;c < repeat_count;++c)
	{
		polynomial_test compose_function = generate();
		polynomial_test compose_1_result = compose_function(anihilator.arg_variables, anihilator.arg_polynomials);
		polynomial_test compose_2_result = anihilator.apply(compose_function);
		assert(compose_1_result == compose_2_result, 
			"test_anihilator failed seed=%u, variables_max=%d, terms_max=%d, total_max=%d, coefficient_max=%d",
			seed, variables_max, terms_max, total_max, coefficient_max
		);
	}
}

template<class coefficient_number>
void test_applicator()
{
	int count = 100;
	uint seed = rand_quick_seed;
	std::cout << "test_applicator " << count << " random cases";
	time_t t = time(nullptr);
	for (int c = 0;c < count;++c)
	{
		if (time(nullptr) - t > 3)
		{
			std::cout << " " << c;
			t = time(nullptr);
		}
		test_applicator<coefficient_number>(++seed, 3, 3, 7, 10, 100);
	}
	std::cout << std::endl;
}

template<>
void test_applicator<coefficient_int>()
{
	int count = 100;
	uint seed = rand_quick_seed;
	std::cout << "test_applicator " << count << " random cases";
	time_t t = time(nullptr);
	for (int c = 0;c < count;++c)
	{
		if (time(nullptr) - t > 3)
		{
			std::cout << " " << c;
			t = time(nullptr);
		}
		test_applicator<coefficient_int>(++seed, 3, 3, 3, 10, 100);
	}
	std::cout << std::endl;
}

template<class coefficient_number>
void test_compose()
{	
	int count = 1000;
	uint seed = rand_quick_seed;
	std::cout << "test_compose " << count << " random cases";
	time_t t = time(nullptr);
	for (int c = 0;c < count;++c)
	{
		if (time(nullptr) - t > 3)
		{
			std::cout << " " << c;
			t = time(nullptr);
		}
		test_compose<coefficient_number>(++seed, 3, 5, 5, 5);
	}
	std::cout << std::endl;

	count = 10;
	seed = rand_quick_seed;
	std::cout << "test_compose " << count << " random bigger cases";
	t = time(nullptr);
	for (int c = 0;c < count;++c)
	{
		if (time(nullptr) - t > 3)
		{
			std::cout << " " << c;
			t = time(nullptr);
		}
		test_compose<coefficient_number>(++seed, 3, 10, 10, 100);
	}
	std::cout << std::endl;
}

template<>
void test_compose<coefficient_int>()
{
	int count = 1000;
	uint seed = rand_quick_seed;
	std::cout << "test_compose " << count << " random cases";
	time_t t = time(nullptr);
	for (int c = 0;c < count;++c)
	{
		if (time(nullptr) - t > 3)
		{
			std::cout << " " << c;
			t = time(nullptr);
		}
		test_compose<coefficient_int>(++seed, 3, 3, 3, 5);
	}
	std::cout << std::endl;
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


template<class coefficient_number>
void test_exponents(int variables_count, int total_max)
{
	using polynomial_test = polynomial<int, coefficient_number>;

	std::vector<polynomial_test> testV;
	polynomial_test testP = 1;
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

template<class coefficient_number>
void test_exponents()
{
	std::cout << "test_exponents";
	for (int variables_count = 1;variables_count <= 5; ++variables_count)
		for (int total_max = 1;total_max <= 5; ++total_max)
			test_exponents<coefficient_number>(variables_count, total_max);
	std::cout << std::endl;
}

template<class coefficient_number>
void test_anihilate(uint seed, int variables_max, int terms_max, int total_max, int coefficient_max)
{
	using polynomial_test = polynomial<int, coefficient_number>;
	using anihilator_test = polynomials_anihilator<int, coefficient_number>;

	rand_quick rand_quick(seed);
	int variables_count = rand_quick(1, variables_max);
	int terms_count = rand_quick(1, terms_max);
	anihilator_test polynomials_anihilator;
	auto generate = [=, &rand_quick]()->polynomial_test
	{
		return random_polynomial<coefficient_number>(rand_quick, terms_count, variables_count, total_max, coefficient_max);
	};
	std::generate_n(std::back_inserter(polynomials_anihilator.arg_polynomials), variables_max + 1, generate);

	assert(polynomials_anihilator.anihilate() && polynomials_anihilator.res_test(),
		"test_anihilate failed seed=%u, variables_max=%d, terms_max=%d, total_max=%d, coefficient_max=%d",
		seed, variables_max, terms_max, total_max, coefficient_max
	);
}

template<class coefficient_number>
void test_anihilate()
{
	const int count = 1000;
	std::cout << "test_anihilate " << count << " random cases";
	auto t = time(nullptr);
	uint seed = rand_quick_seed;
	for (int c = 0;c < count;++c)
	{
		if (time(nullptr) - t> 3)
		{
			std::cout << " " << c;
			t = time(nullptr);
		}
		test_anihilate<coefficient_number>(rand_quick_seed+c, 1, 5, 10, 10);
	}
	std::cout << std::endl;
}

template<>
void test_anihilate<coefficient_int>()
{
	const int count = 1000;
	std::cout << "test_anihilate " << count << " random cases";
	auto t = time(nullptr);
	uint seed = rand_quick_seed;
	for (int c = 0;c < count;++c)
	{
		if (time(nullptr) - t > 3)
		{
			std::cout << " " << c;
			t = time(nullptr);
		}
		test_anihilate<coefficient_int>(rand_quick_seed + c, 1, 3, 3, 5);
	}
	std::cout << std::endl;
}


void test_polynomials()
{
	std::cout << "testing coefficient_mod" << std::endl;
	test_parse<coefficient139>();
	test_arithmetic<coefficient139>();
	test_exponents<coefficient139>();
	test_compose<coefficient139>();
	test_applicator<coefficient139>();
	test_anihilate<coefficient139>();	
}