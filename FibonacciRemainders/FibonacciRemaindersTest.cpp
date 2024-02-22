/*
* Calculate Fibonacci sequence remainders for large number and fun.
* Main function is defined uint64_t fibonacci_remainder_by_power(uint64_t fibonacci_number, uint64_t fibonacci_remainder);
* 
* Remainder are quit useful  to make tractable some calculation. Combinatoric for example may provide many calculation but involved with not tractable number. Remainder may convert them down to earth.
* Fibonacci grows very fast making calculation unintrested. Reminder restore question.
* Main fibonacci procedure uses multiplication by matrix and fast exponentation similar to ordinary for number.
* For tests second version fibonacci procedure uses cycle detection and observation that remainder sequence  goes to cycle.
*/

#include <iostream>
#include <random>
#include "FibonacciRemainders.h"

int main()
{
    std::cout << "Hello World!\n";
    void test_all();
    test_all();
    std::cout << "By World!\n";    
}

std::mt19937 gen(1313);
std::uniform_int_distribution<unsigned> dist;
unsigned urand() { return dist(gen);  }

void test_remainder_add()
{
    unsigned count = 1000;

    for (unsigned i = 0; i < count; ++i)
    {
        unsigned i1 = urand(), i2 = urand(), i3 = urand();
        unsigned i4 = (uint64_t(i1) + uint64_t(i2)) % uint64_t(i3);
        unsigned i5 = remainder_add32(i1, i2, i3);
        assert(i4 == i5, "test_remainder_add");
    }
}

void test_remainder_mul()
{
    unsigned count = 1000;

    for (unsigned i = 0; i < count; ++i)
    {
        unsigned i1 = urand(), i2 = urand(), i3 = urand();
        unsigned i4 = (uint64_t(i1) * uint64_t(i2)) % uint64_t(i3);
        unsigned i5 = remainder_mul32(i1, i2, i3);
        assert(i4 == i5, "test_remainder_mul");
    }
}

void test_remainder_pow()
{
    unsigned count = 1000;
    
    for (unsigned c1 = 0; c1 < count; ++c1)
    {
        const unsigned i1 = urand(), i2 = urand() % (16 * 1024), i3 = urand();

        uint64_t i4 = 1;
        for (unsigned c2=0;c2<i2;++c2)
            i4 = i4 * i1 % i3;
        unsigned i5 = remainder_pow32(i1, i2, i3);
        assert(i4 == i5, "test_remainder_pow");
    }
}


void test_remainder_pow4()
{
    unsigned count = 1000;
    
    for (unsigned c1 = 0; c1 < count; ++c1)
    {
        uint64_t i1[2][2] = { urand(), urand() , urand() , urand() };
        const unsigned i2 = urand() % 1024, i3 = urand();
        
        // Manualy simply power matrix i1 to power i2 taking remainder i3;
        uint64_t i4[2][2] = { {1,0}, {0,1} };
        for (unsigned c2 = 0; c2 < i2; ++c2)
        {
            uint64_t i5[2][2] = {
                i4[0][0] * i1[0][0] % i3 + i4[0][1] * i1[1][0] % i3,
                i4[0][0] * i1[0][1] % i3 + i4[0][1] * i1[1][1] % i3,
                i4[1][0] * i1[0][0] % i3 + i4[1][1] * i1[1][0] % i3,
                i4[1][0] * i1[0][1] % i3 + i4[1][1] * i1[1][1] % i3
            };

            i4[0][0] = i5[0][0] % i3;
            i4[0][1] = i5[0][1] % i3;
            i4[1][0] = i5[1][0] % i3;
            i4[1][1] = i5[1][1] % i3;
        }

        uint64_t i5[2][2];
        remainder_pow4(i1, i2, i5, i3);
        assert(i4[0][0] == i5[0][0] && i4[1][0] == i5[1][0] && i4[0][1] == i5[0][1] && i4[1][1] == i5[1][1], "test_remainder_pow4");
    }
}

void test_fibonacci_by_power()
{
    const int fibonacci_remainders = 128, fibonacci_numbers = 1024;

    for (int i1 = 2; i1 < fibonacci_remainders; ++i1)
    {
        uint64_t fibonacci_curr = 1;
        uint64_t fibonacci_test = fibonacci_remainder_by_power(0, i1);
        assert(fibonacci_test == fibonacci_curr, "test_fibonacci_by_power");

        fibonacci_curr = 1;
        fibonacci_test = fibonacci_remainder_by_power(1, i1);
        assert(fibonacci_test == fibonacci_curr, "test_fibonacci_by_power");

        uint64_t fibonacci_prev1 = 1, fibonacci_prev2 = 1;
        for (int i2 = 2; i2 < fibonacci_numbers; ++i2)
        {
            uint64_t fibonacci_curr = (fibonacci_prev1 + fibonacci_prev2) % i1;
            fibonacci_prev2 = fibonacci_prev1;
            fibonacci_prev1 = fibonacci_curr;
            fibonacci_test = fibonacci_remainder_by_power(i2, i1);
            assert(fibonacci_test == fibonacci_curr, "test_fibonacci_by_power");
        }
    }
}

void test_fibonacci_by_cycle()
{
    const int fibonacci_remainders = 128, fibonacci_numbers = 1024;

    for (int i1 = 2; i1 < fibonacci_remainders; ++i1)
    {
        uint64_t fibonacci_curr = 1;
        uint64_t fibonacci_test = fibonacci_remainder_by_power(0, i1);
        assert(fibonacci_test == fibonacci_curr, "test_fibonacci_by_cycle");

        fibonacci_curr = 1;
        fibonacci_test = fibonacci_remainder_by_power(1, i1);
        assert(fibonacci_test == fibonacci_curr, "test_fibonacci_by_cycle");

        uint64_t fibonacci_prev1 = 1, fibonacci_prev2 = 1;
        for (int i2 = 2; i2 < fibonacci_numbers; ++i2)
        {
            uint64_t fibonacci_curr = (fibonacci_prev1 + fibonacci_prev2) % i1;
            fibonacci_prev2 = fibonacci_prev1;
            fibonacci_prev1 = fibonacci_curr;
            fibonacci_test = fibonacci_remainder_by_cycle(i2, i1);
            assert(fibonacci_test == fibonacci_curr, "test_fibonacci_by_cycle");
        }
    }
}

void test_fibonacci_value()
{    
    int count = 1000;
   std::mt19937 gen(1313);
   std::uniform_int_distribution<unsigned> dist(1000,1000000);
    auto urand = [&gen, &dist] { return dist(gen);  };

    for (int i = 0; i < count; ++i)
    {
        uint64_t fibonacci_number = urand();
        uint64_t fibonacci_remainder = urand();
        uint64_t fibonacci_by_cycle = fibonacci_remainder_by_cycle(fibonacci_number, fibonacci_remainder);
        uint64_t fibonacci_by_power = fibonacci_remainder_by_power(fibonacci_number, fibonacci_remainder);
        assert(fibonacci_by_cycle == fibonacci_by_power, "test_fibonacci_value");
    }
}

void test_fibonacci_speed()
{
    int count = 1000;
    std::mt19937 gen(1313);
    uint64_t test_range = 1000000;
    test_range = test_range * test_range * test_range;
    std::uniform_int_distribution<uint64_t> dist(test_range, 2* test_range);
    auto rand = [&gen, &dist] { return dist(gen);  };

    for (int i = 0; i < count; ++i)
    {
        uint64_t fibonacci_number = urand();
        uint64_t fibonacci_remainder = urand();     
        uint64_t fibonacci_by_power = fibonacci_remainder_by_power(fibonacci_number, fibonacci_remainder);        
    }
}

void test_all()
{   
    test_remainder_mul();
    test_remainder_add();
    test_remainder_pow();
    test_remainder_pow4();
    test_fibonacci_by_power();
    test_fibonacci_by_cycle();
    test_fibonacci_value();
    test_fibonacci_speed();
}

