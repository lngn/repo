#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include "rand_quick.h"

template<class exponent_number>
class polynomial_exponent
{
public:
    char variable = 'x';
    exponent_number exponent = 1;

    polynomial_exponent(char variable = 'x', exponent_number exponent = 1) 
        : variable(variable), exponent(exponent)
    {
    }

    bool operator<(const polynomial_exponent& arg) const
    {
        return variable < arg.variable || variable == arg.variable && exponent < arg.exponent;
    }

    bool operator<=(const polynomial_exponent& arg) const
    {
        return variable < arg.variable || variable == arg.variable && exponent <= arg.exponent;
    }

    bool operator==(const polynomial_exponent& arg) const
    {
        return variable == arg.variable && exponent == arg.exponent;
    }

    bool operator!=(const polynomial_exponent& arg) const {
        return variable != arg.variable || exponent != arg.exponent;
    }
};

template<class exponent_number>
class polynomial_exponents: public std::vector<polynomial_exponent<exponent_number>>
{
public:
    polynomial_exponents() {}
    polynomial_exponents(std::vector<polynomial_exponent<exponent_number>>&& exponents)
        : std::vector<polynomial_exponent<exponent_number>>(exponents)
    {
    }

    void normalize()
    {
        // order variables decreasing
        std::sort(this->begin(), this->end(), [](const auto& ala, const auto& bob) { return bob < ala;  });
        // three iterators read, write, end to compress multi occured variable e.g x^2*y^3*x^2
        auto itR = this->begin(), itW = itR, itE = this->end();
        if (itR == itE)
            return;
        if (++itR == itE)
            return;
        // scan for first reoccuring variable ( due to sort this could be only next in sequence)
        do
        {
            // found first reoccured variable start compressing
            if (itW->variable == itR->variable)
            {
                do
                {
                    if (itW->variable == itR->variable)
                        itW->exponent += itR->exponent;
                    else
                        *(++itW) = *itR;

                } while (++itR != itE);
                // remove tail of already accumulated variables and finish
                this->erase(++itW, itE);
                break;
            }
            itW = itR;
        } while (++itR != itE);
    }

    bool normalized() const
    {
        // three iterators read, write, end to compress multi occured variable e.g x^2*y^3*x^2
        auto itR = this->begin(), itW = itR, itE = this->end();

        if (itR == itE || ++itR == itE)
            return true;
        // scan for first reoccuring variable ( due to sort this could be only next in sequence)
        do
        {
            if (*itW <= *itR)
                return false;
        } while ( ++itW,++itR != itE );

        return true;
    }

    // total (summed) exponents
    int total() const
    {
        int total = 0;
        for (const auto& exponent : *this)
            total += exponent.exponent;
        return total;
    }

    bool operator<(const polynomial_exponents<exponent_number>& that) const 
    {
        // first test total exponent
        auto thisT = this->total(), thatT = that.total();
        if (thisT != thatT)
            return thisT < thatT;
        // total exponent are equals go through list for different exponent
        for (auto thisI = this->begin(), thisE = this->end(), thatI = that.begin(), thatE = that.end();thisI != thisE && thatI != thatE;++thisI, ++thatI)
            if (*thisI != *thatI)
                return *thisI < *thatI;
        return false;
    }

    bool operator<=(const polynomial_exponents<exponent_number>& that) const
    {
        // first test total exponent
        auto thisT = this->total(), thatT = that.total();
        if (thisT != thatT)
            return thisT <= thatT;
        // total exponent are equals go through list for different exponent
        for (auto thisI = this->begin(), thisE = this->end(), thatI = that.begin(), thatE = that.end();thisI != thisE && thatI != thatE;++thisI, ++thatI)
            if (*thisI != *thatI)
                return *thisI < *thatI;
        return true;
    }

    bool operator==(const polynomial_exponents<exponent_number>& that) const
    {
        // first test total exponent
        auto thisT = this->total(), thatT = that.total();
        if (thisT != thatT)
            return false;
        // total exponent are equals go through list for different exponent
        for (auto thisI = this->begin(), thisE = this->end(), thatI = that.begin(), thatE = that.end();thisI != thisE && thatI != thatE;++thisI, ++thatI)
            if (*thisI != *thatI)
                return false;
        return true;
    }

    bool operator!=(const polynomial_exponents<exponent_number>& that) const
    {
        // first test total exponent
        auto thisT = this->total(), thatT = that.total();
        if (thisT != thatT)
            return true;
        // total exponent are equals go through list for different exponent
        for (auto thisI = this->begin(), thisE = this->end(), thatI = that.begin(), thatE = that.end();thisI != thisE && thatI != thatE;++thisI, ++thatI)
            if (*thisI != *thatI)
                return true;
        return false;
    }
    
    polynomial_exponents<exponent_number>& operator++()
    {
        auto itB = this->begin(), itL = this->end();
        if (itB ==itL) 
            return *this;
        if (--itL == itB)
        {
            ++itB->exponent;
            return *this;
        }

        auto it = itL;
        while (it != itB && it->exponent == 0) --it;

        if (it == itB) 
        {
            itL->exponent = itB->exponent + 1;
            itB->exponent = 0;
            
        }
        else
        {
            itL->exponent = it->exponent - 1;
            if (it != itL )
                it->exponent = 0;
            ++(--it)->exponent;
        }
        return *this;
    }

    polynomial_exponents<exponent_number>& operator--()
    {
        auto itB = this->begin(), itL = this->end();
        if (itB == itL)
            return *this;
        if (--itL == itB)
        {
            --itB->exponent;
            return *this;
        }

        auto it = itL;
        do --it; while (it != itB && it->exponent == 0);

        if (it->exponent == 0)
        {
            itB->exponent = itL->exponent - 1;
            itL->exponent = 0;

        }
        else
        {
            --it->exponent;
            (++it)->exponent = itL->exponent + 1;
            if (it != itL)
                itL->exponent = 0;
        }

        return *this;
    }
};

namespace std {
    template <class exponent_number> struct hash<polynomial_exponents<exponent_number>>
    {
        size_t operator()(const polynomial_exponents<exponent_number>& exponents) const
        {
            size_t hash = 0;
            for (const auto& exponent : exponents)
                hash ^= rand_quick::next(exponent.variable) ^ rand_quick::next(exponent.exponent);
            return hash;
        }
    };
}

template<class exponent_number, class coefficient_number>
class polynomial_coefficient
{
public:
    coefficient_number coefficient;
    polynomial_exponents<exponent_number> exponents;

    polynomial_coefficient(coefficient_number coefficient)
        : coefficient(coefficient)
    {
    }
    polynomial_coefficient(coefficient_number coefficient, char variable, exponent_number exponent)
       : coefficient(coefficient)
       , exponents({ polynomial_exponent<exponent_number>(variable, exponent)})
    {
    }
    polynomial_coefficient(coefficient_number coefficient, std::vector<polynomial_exponent<exponent_number>> && exponents)
        : exponents(exponents)
        , coefficient(coefficient)
    {
    }

    void normalize() { exponents.normalize(); }
    bool normalized() const { return coefficient !=0 && exponents.normalized(); }
    int total() { return exponents.total();  }

    bool operator<(const polynomial_coefficient& arg) const
    {
        return exponents < arg.exponents || exponents == arg.exponents && coefficient < arg.coefficient;
    }
    bool operator<=(const polynomial_coefficient& arg) const
    {
        return exponents < arg.exponents || exponents == arg.exponents && coefficient <= arg.coefficient;
    }
    bool operator==(const polynomial_coefficient& arg) const
    {
        return exponents == arg.exponents && coefficient == arg.coefficient;
    }
    bool operator!=(const polynomial_coefficient& arg) const
    {
        return exponents != arg.exponents || coefficient != arg.coefficient;
    }

    polynomial_coefficient operator+(const polynomial_coefficient& that) const
    {
        polynomial_coefficient<exponent_number, coefficient_number> result = this->coefficient + that.coefficient;
        result.exponents =  this->exponents;
        return result;
    }

    polynomial_coefficient operator-() const
    {
        polynomial_coefficient<exponent_number, coefficient_number> result = -this->coefficient;
        result.exponents = this->exponents;
        return result;
    }

    polynomial_coefficient operator-(const polynomial_coefficient& that) const
    {
        polynomial_coefficient<exponent_number, coefficient_number> result = this->coefficient - that.coefficient;
        result.exponents = this->exponents;
        return result;
    }

    polynomial_coefficient operator*(const polynomial_coefficient & that) const
    {
        polynomial_coefficient<exponent_number, coefficient_number> result(coefficient * that.coefficient);
        auto thisI = exponents.begin(), thisE = exponents.end(), thatI = that.exponents.begin(), thatE = that.exponents.end();
        while (thisI != thisE && thatI != thatE)
        {
            if (thisI->variable == thatI->variable)
            {
                result.exponents.emplace_back(thisI->variable, thisI->exponent + thatI->exponent);
                ++thisI;
                ++thatI;
            }
            else if (thisI->variable < thatI->variable)
            {
                result.exponents.emplace_back(thatI->variable, thatI->exponent);
                ++thatI;
            }
            else
            {
                result.exponents.emplace_back(thisI->variable, thisI->exponent);
                ++thisI;
            }
        }
        while (thisI != thisE)
        {
            result.exponents.emplace_back(thisI->variable, thisI->exponent);
            ++thisI;
        }
        while (thatI != thatE)
        {
            result.exponents.emplace_back(thatI->variable, thatI->exponent);
            ++thatI;
        }
        return result;
    }
};

template<class exponent_number, class coefficient_number>
class polynomial: public std::vector<polynomial_coefficient<exponent_number, coefficient_number>>
{
public:
    polynomial() 
    {
    };

    polynomial(coefficient_number coefficient)
        : std::vector<polynomial_coefficient<exponent_number, coefficient_number>>({ polynomial_coefficient<exponent_number,coefficient_number>(coefficient) })
    {
    };
    polynomial(char variable)
        : std::vector<polynomial_coefficient<exponent_number, coefficient_number>>({ polynomial_coefficient<exponent_number, coefficient_number>(1, variable,1) })
    {
    };
    polynomial(std::vector<polynomial_coefficient<exponent_number, coefficient_number>> && coefficients)
        : std::vector<polynomial_coefficient<exponent_number, coefficient_number>>(coefficients)
    {
        normalize(true);
    }

    void normalize(bool normalize_coefficients = false)
    {
        // three iterators read, write, end to compress multi occured terms
        auto itR = this->begin(), itW = itR, itE = this->end();
        if (normalize_coefficients)
        {
            for (auto& coefficient : *this)
                coefficient.normalize();
        }

        // order polynomial terms decreasing
        std::sort( itR, itE, [](const auto& ala, const auto& bob) { return bob.exponents < ala.exponents;  });
        // test simple cases 0 or 1 term and gather variables
        if (itR == itE)
            return;
        if (++itR == itE)
        {
            if (itW->coefficient == 0)
                this->clear();
            return;
        }

        // merge reoccuring coefficients e.g. 2*x^2, 3*x^2 -> 5*x^2
        do
        {
            // test first reoccuring terms
            if (itW->exponents == itR->exponents || itW->coefficient == 0)
            {
                do
                {
                    if (itW->exponents == itR->exponents)
                        itW->coefficient += itR->coefficient;
                    else if (itW->coefficient == 0)
                        *itW = *itR;
                    else
                        *(++itW) = *itR;
                } while (++itR != itE);
                break;
            }
            itW = itR;
        } while (++itR != itE);

        // remove tail of already accumulated coefficients
        if (itW->coefficient != 0)
            ++itW;
        this->erase(itW, itE);
    }

    bool normalized() const
    {
        // test each coefficient is normalized ( variables in term are decreasing and not reoccur)
        for (auto& coefficient : *this)
            if (!coefficient.normalized())
                return false;
        // test polynomial terms are ordered decreasing        
        auto itR = this->begin(), itW = itR, itE = this->end();
        if (itR == itE)
            return true;
        if (++itR == itE)
            return true;
        
        // scan terms for reoccuring 
        do
        {
            if (itW->exponents < itR->exponents)
                return false;
        } while (++itW, ++itR != itE);

        return true;
    }

    bool operator==(const polynomial<exponent_number, coefficient_number>& that) const
    {
        if (this->size() != that.size())
            return false;
        for (auto thisI = this->begin(), thisE = this->end(), thatI = that.begin(), thatE = that.end();thisI != thisE;++thisI, ++thatI)
            if (*thisI != *thatI)
                return false;
        return true;
    }

    bool operator!=(const polynomial<exponent_number, coefficient_number>& that) const
    {
        if (this->size() != that.size())
            return true;
        for (auto thisI = this->begin(), thisE = this->end(), thatI = that.begin(), thatE = that.end();thisI != thisE;++thisI, ++thatI)
            if (*thisI != *thatI)
                return true;
        return false;
    }

    bool operator<(const polynomial<exponent_number, coefficient_number>& that) const
    {
        
        for (auto thisI = this->begin(), thisE = this->end(), thatI = that.begin(), thatE = that.end();;++thisI, ++thatI)
        {
            if (thisI == thisE)
                return thatI != thatE;
            if (thatI == thatE)
                return false;
            if (*thisI != *thatI)
                return *thisI<*thatI;
        }
        return false;
    }

    bool operator<=(const polynomial<exponent_number, coefficient_number>& that) const
    {
        for (auto thisI = this->begin(), thisE = this->end(), thatI = that.begin(), thatE = that.end();;++thisI, ++thatI)
        {
            if (thisI == thisE)
                return true;
            if (thatI == thatE)
                return false;
            if (*thisI != *thatI)
                return *thisI <= *thatI;
        }
        return true;
    }

    polynomial<exponent_number, coefficient_number> operator+(const polynomial<exponent_number, coefficient_number>& that) const 
    {
        polynomial<exponent_number, coefficient_number> result;
        auto thisI = this->begin(), thisE = this->end(), thatI = that.begin(), thatE = that.end();
        while (thisI != thisE && thatI != thatE)
        {
            if (thisI->exponents == thatI->exponents)
            {
                if (thisI->coefficient + thatI->coefficient != 0)
                    result.emplace_back(*thisI + *thatI);
                ++thisI;
                ++thatI;
            }
            else if (thisI->exponents < thatI->exponents)
            {
                result.push_back(*thatI);
                ++thatI;
            }
            else
            {
                result.push_back(*thisI);
                ++thisI;
            }
        }
        while (thisI != thisE)
        {
            result.push_back(*thisI);
            ++thisI;
        }
        while (thatI != thatE)
        {
            result.push_back(*thatI);
            ++thatI;
        }
        return result;
    }

    polynomial<exponent_number, coefficient_number> operator-() const
    {
        polynomial<exponent_number, coefficient_number> result;
        auto thisI = this->begin(), thisE = this->end();
        while (thisI != thisE)
        {
            result.push_back(-*thisI);
            ++thisI;
        }        
        return result;
    }

    polynomial<exponent_number, coefficient_number> operator-(const polynomial<exponent_number, coefficient_number>& that) const
    {
        polynomial<exponent_number, coefficient_number> result;
        auto thisI = this->begin(), thisE = this->end(), thatI = that.begin(), thatE = that.end();
        while (thisI != thisE && thatI != thatE)
        {
            if (thisI->exponents == thatI->exponents)
            {
                if (thisI->coefficient - thatI->coefficient !=0)
                    result.emplace_back(*thisI - *thatI);
                ++thisI;
                ++thatI;
            }
            else if (thisI->exponents < thatI->exponents)
            {
                result.push_back(-*thatI);
                ++thatI;
            }
            else
            {
                result.push_back(*thisI);
                ++thisI;
            }
        }
        while (thisI != thisE)
        {
            result.push_back(*thisI);
            ++thisI;
        }
        while (thatI != thatE)
        {
            result.push_back(-*thatI);
            ++thatI;
        }
        return result;
    }

    polynomial<exponent_number, coefficient_number> operator*(const polynomial<exponent_number, coefficient_number>& that) const
    {
        polynomial<exponent_number, coefficient_number> result;
        for (auto & thisV : *this)
            for (auto & thatV: that)
                result.push_back(thisV*thatV);
        result.normalize();
        return result;
    }

    polynomial<exponent_number, coefficient_number> operator^(int exponent) const
    {
        if (exponent == 0)
            return 1;
        else if (exponent == 1)
            return *this;
        
        polynomial<exponent_number, coefficient_number> result = exponent & 1 ? *this : 1, power2=*this;
        for (exponent >>= 1; exponent != 0; exponent >>= 1)
        {
            power2 = power2 * power2;
            if (exponent & 1)
                result = result * power2;
        }
        return result;
    }
    
    polynomial<exponent_number, coefficient_number> operator()(
        const std::vector<char>& variables, 
        const std::vector<std::reference_wrapper<polynomial<exponent_number, coefficient_number>>>& polynomials)
    {
        polynomial<exponent_number, coefficient_number> result;
        // Calculation of single term in result polynomial
        polynomial<exponent_number, coefficient_number> term;
        // Variables in source term not matching any source polynomial will be multiplate as is to target. Partial evaluation for example.
        polynomial<exponent_number, coefficient_number> rest;

        for (auto& thisV : *this)
        {
            term = thisV.coefficient;
            rest = 1;
            for (auto& exponent : thisV.exponents)
            {
                auto it = std::find(variables.begin(), variables.end(), exponent.variable);
                if (it != variables.end())
                { 
                    auto polynomial_index = it - variables.begin();
                    const polynomial<exponent_number, coefficient_number> & polynomial = polynomials[polynomial_index];
                    term = term * (polynomial ^ exponent.exponent);
                }
                else
                {
                    rest.front().exponents.push_back(polynomial_exponent<exponent_number>(exponent));
                }
                    
            }
            if (!rest.front().exponents.empty())
                term = term * rest;
            result = result + term;
        }
        result.normalize();
        return result;
    }

    std::string string() const
    {
        std::ostringstream stream;
        if (std::vector<polynomial_coefficient<exponent_number, coefficient_number>>::empty())
        {
            stream << 0;
            return stream.str();
        }
        for (auto & coefficient : *this)
        {
            if (coefficient.exponents.empty())
            {
                if (stream.tellp() > 0 && coefficient.coefficient > 0)
                    stream << "+";
                stream << coefficient.coefficient;
            }
            else
            {
                if (coefficient.coefficient == 1)
                {
                    if (stream.tellp() > 0)
                        stream << "+";
                }
                else if (coefficient.coefficient == -1)
                {
                    stream << "-";
                }
                else
                {
                    if (stream.tellp() > 0 && coefficient.coefficient > 0)
                        stream << "+";
                    stream << coefficient.coefficient << "*";;
                }
                for (auto it = coefficient.exponents.begin(), itE = coefficient.exponents.end();;)
                {
                    stream << it->variable;
                    if (it->exponent != 1)
                        stream << it->exponent;
                    if (++it == itE)
                        break;
                    stream << "*";
                }
            }

           
        }
        return stream.str();
    };

    bool parse(const std::string& string)
    {
        this->clear();

        int coefficient = 0, exponent;
        char variable;
        polynomial_coefficient<exponent_number, coefficient_number> polynomial_coefficient = 0;

        const char* p = string.c_str();
        auto skip = [](const char*& p) { while (*p && isspace(*p)) ++p; return *p != 0;};

        for(;;)
        {
            if (!skip(p))
            {
                if (coefficient != 0)
                    this->push_back(polynomial_coefficient);
                normalize(true);
                return true;
            }

            if (coefficient == 0)
            {
                skip(p);
                if (*p == '+')
                {
                    coefficient = 1;
                    ++p;
                }
                else if (*p == '-')
                {
                    coefficient = -1;
                    ++p;
                }
                else
                {
                    coefficient = 1;
                }
                skip(p);
                if ('0' <= *p && *p <= '9')
                {
                    coefficient *= strtol(p, const_cast<char**>(&p), 10);
                }
                polynomial_coefficient = coefficient;
            }
            else if (*p == '+' || *p == '-')
            {
                this->push_back(polynomial_coefficient);
                coefficient = 0;
            }
            else if (*p == '*')
            {
                skip(++p);
                if ('a' <= *p && *p <= 'z')
                {
                    exponent = 1;
                    variable = *p;
                    skip(++p);
                    if ('0' <= *p && *p <= '9')
                    {
                        exponent = strtol(p, const_cast<char**>(&p), 10);
                        if (exponent == 0)
                            return false;

                    }
                }
                else return false;
                polynomial_coefficient.exponents.emplace_back(variable, exponent);
            }
            else if ('a' <= *p && *p <= 'z')
            {
                exponent = 1;
                variable = *p;
                skip(++p);
                if ('0' <= *p && *p <= '9')
                {
                    exponent = strtol(p, const_cast<char**>(&p), 10);
                    if (exponent == 0)
                        return false;

                }
                polynomial_coefficient.exponents.emplace_back(variable, exponent);
            }
            else return false;
        }
    }
};

template<class exponent_number, class coefficient_number>
polynomial<exponent_number, coefficient_number> operator+(coefficient_number left, const polynomial<exponent_number, coefficient_number>& right)
{
    return right + left;
}

template<class exponent_number, class coefficient_number>
polynomial<exponent_number, coefficient_number> operator-(coefficient_number left, const polynomial<exponent_number, coefficient_number>& right)
{
    return -right + left;
}

template<class exponent_number, class coefficient_number>
polynomial<exponent_number, coefficient_number> operator*(coefficient_number left, const polynomial<exponent_number, coefficient_number>& right)
{
    return right * left;
}

template <class exponent_number, class coefficient_number>
class polynomials_applicator
{
public:
    std::vector<char> variables;
    std::vector<polynomial< exponent_number, coefficient_number >> polynomials;

    polynomial<exponent_number, coefficient_number> apply(const polynomial< exponent_number, coefficient_number >& thisP)
    {
        polynomial<exponent_number, coefficient_number> result;
        // Calculation of single term in result polynomial
        polynomial<exponent_number, coefficient_number> term;
        // Variables in source term not matching any source polynomial will be multiplate as is to target. Partial evaluation for example.
        polynomial<exponent_number, coefficient_number> rest;

        for (auto& thisV : thisP)
        {
            term = thisV.coefficient;
            rest = 1;
            for (auto& exponent : thisV.exponents)
            {
                auto it = std::find(variables.begin(), variables.end(), exponent.variable);
                if (it != variables.end())
                {
                    auto polynomial_index = it - variables.begin();
                    term = term * apply(polynomial_index, exponent.exponent);
                }
                else
                {
                    rest.front().exponents.push_back(polynomial_exponent<exponent_number>(exponent));
                }

            }
            if (!rest.front().exponents.empty())
                term = term * rest;
            result = result + term;
        }
        result.normalize();
        return result;
    }
private:
    std::vector<std::vector<polynomial< exponent_number, coefficient_number >>> cache_powers;

    polynomial<exponent_number, coefficient_number>& apply(int index, int exponent)
    {
        std::vector<polynomial< exponent_number, coefficient_number >>& cache = cache_powers[index];
        while (cache.size() <= exponent)
            cache.emplace_back(0);
        if (cache[index] == 0)
        {
            if (cache[index - 1] != 0)
                cache[index] = cache[index - 1] * cache[1];
            else
                cache[index] = apply(index, exponent / 2) * apply(index, (exponent + 1) / 2);
        }
        return cache[index];
    }
};

template <class exponent_number, class coefficient_number>
class polynomials_anihilator: polynomials_applicator<exponent_number, coefficient_number>
{
public:
    enum Status 
    {
        status_started,
        status_total_exponents_overflow,
        status_finished
    } status;
    int total_exponents_max = 10;
    int total_coefficients_max = 1000;

    bool anihilate2()
    {

        polynomial<exponent_number, coefficient_number> compose_function = 1, evaluated_function;
        for (char variable : this->variables)
            compose_function.front().exponents.push_back(variable);
        for (;;)
        {
            if (++compose_function > total_exponents_max)
            {

            }
            composition composition
            {
                compose_function = compose_function,
                evaluated_function = apply(compose_function)
            };
            
            auto it = cache_compositions.find(composition.evaluated_function.front().exponents);
            if (it == cache_compositions.end())
            {
                cache_compositions[composition.evaluated_function.front().exponents] = composition;
                continue;
            }
            for (;;)
            {
                composition.compose_function = composition.compose_function* it->second.compose_function.front().coefficient - it->second.compose_function * composition.compose_function.front().coefficient;

            }
            if (composition.evaluated_function == 0)
            {

            }
        }


        //
        for (int total_exponent = 0; ;++total_exponent)
        {
            

            for (int exponent = 0;exponent <= total_exponent;++exponent)
            {
                composition compositionN;
            }
        }
    }
private:
    struct composition
    {
        polynomial<exponent_number, coefficient_number> compose_function;
        polynomial<exponent_number, coefficient_number> evaluated_function;
    };
    std::unordered_map<polynomial_exponents<exponent_number>,composition> cache_compositions;
};