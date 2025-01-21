#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

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
};

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
    bool normalized() const { return exponents.normalized(); }

    bool operator<(const polynomial_coefficient& arg) 
    {
        return exponents < arg.exponents || exponents == arg.exponents && coefficient < arg.coefficient;
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
            return;

        // merge reoccuring coefficients e.g. 2*x^2, 3*x^2 -> 5*x^2
        do
        {
            // test first reoccuring terms
            if (itW->exponents == itR->exponents)
            {
                do
                {
                    if (itW->exponents == itR->exponents)
                        itW->coefficient += itR->coefficient;
                    else
                        *(++itW) = *itR;
                } while (++itR != itE);
                // remove tail of already accumulated coefficients
                this->erase(++itW, itE);
                break;
            }
            itW = itR;
        } while (++itR != itE);
    }

    bool normalized(bool normalized_coefficients = false) const
    {
        // test each coefficient is normalized ( variables in term are decreasing and not reoccur)
        if (normalized_coefficients)
        {
            for (auto& coefficient : *this)
                if (!coefficient.normalized())
                    return false;
        }
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

    polynomial<exponent_number, coefficient_number> operator+(const polynomial<exponent_number, coefficient_number>& that) const 
    {
        polynomial<exponent_number, coefficient_number> result;
        auto thisI = this->begin(), thisE = this->end(), thatI = that.begin(), thatE = that.end();
        while (thisI != thisE && thatI != thatE)
        {
            if (thisI->exponents == thatI->exponents)
            {
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
    
    polynomial operator()(
        const std::vector<char>& variables, 
        const std::vector<std::reference_wrapper<polynomial<exponent_number, coefficient_number>>>& polynomials)
    {
        polynomial<exponent_number, coefficient_number> result;
        for (auto& thisV : *this)
        {
            polynomial<exponent_number, coefficient_number> term(thisV.coefficient);
            for (auto& exponent : thisV.exponent)
            {
                auto it = std::find(variables.begin(), variables.end(), exponent.variable);
                if (it == variables.end())
                    term *= exponent;
                else term *= polynomials[it - variables.begin()] ^ exponent->exponent;
            }
            result += term;
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
            if (coefficient.coefficient == 1 && !coefficient.exponents.empty())
            {
                if (stream.tellp() > 0)
                    stream << "+";
            }
            else if (coefficient.coefficient == -1 && !coefficient.exponents.empty())
            {
                stream << "-";
            }
            else
            {
                if (stream.tellp() > 0 && coefficient.coefficient > 0)
                    stream << "+";
                stream << coefficient.coefficient;
            }
            for (auto & exponent : coefficient.exponents)
            {
                if (coefficient.coefficient != 1 && coefficient.coefficient != -1)
                    stream << "*";
                stream << exponent.variable;
                if (exponent.exponent != 1)
                    stream << exponent.exponent;
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

    bool anihilate2(const polynomial & ala, const polynomial & bob)
    {
        std::vector<polynomial> ala_powers;
        std::vector<polynomial> bob_powers;

        //
        for (int total_exponent = 0; ;++total_exponent)
        {
            ala_powers.push_back(polynomial<exponent_number, coefficient_number>().mul(ala_powers.back(), ala));
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