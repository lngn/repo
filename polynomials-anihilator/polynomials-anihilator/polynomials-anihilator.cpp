﻿#include <cstdio>
#include "polynomial.h"
#include <ranges>
#include <list>
int main()
{
    std::vector<int> t{ 1,2 };
    auto s = (++t.begin()) - (t.begin());
    auto ss = t[1];

    // requires /std:c++20
    std::vector<int> input = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto output = input
        | std::views::filter([](const int n) {return n % 3 == 0; })
        | std::views::transform([](const int n) {return n * n; });

    void test_parse();
    void test_arithmetic();
    void test_compose();

    test_compose();
    test_parse();
    test_arithmetic();
    

    std::cout << "done";
    std::cin.get();
}