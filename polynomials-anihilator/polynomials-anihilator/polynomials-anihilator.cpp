#include <cstdio>
#include "polynomial.h"


int main01()
{
    polynomial_exponent<int> t1 = {'x', 5};
    polynomial_coefficient<int, double> t2(3,t1);
    polynomial_coefficient<int, double> t3(4, t1);

    polynomial<int, double> ala01(t2);
    std::vector<polynomial_coefficient<int, double> > v1 = { t2, t3 };
    polynomial<int, double> ala03({ t2, t3 });
    printf(ala01.string().c_str());

    polynomial<int, double> ala({ 1, {'a', 1} });

    polynomial<int, double> ala02;
    ala02 = ala;

    printf(ala.string().c_str());
    return 0;
}

int main()
{
    void test_parse();
    void test_arithmetic();
    test_arithmetic();
    test_parse();

    std::cout << "done";
    std::cin.get();
}