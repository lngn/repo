
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

const uint64_t first_fibonacci_number = 1;
const uint64_t second_fibonacci_number = 1;
const uint64_t first_fibonacci_coefficient = 1;
const uint64_t second_fibonacci_coefficient = 1;

void assert(int value, const char* message)
{
    if (value == 0)
    {
        printf(message);
        exit(1);
    }
}

uint32_t remainder_add32(uint32_t first, uint32_t second, uint32_t remainder)
{
    while (second  > UINT32_MAX - first)
    {
        first = (first + second) % remainder;
        second = UINT32_MAX % remainder + 1;
    }
    return (first + second) % remainder;
}

uint32_t remainder_mul32(uint32_t first, uint32_t second, uint32_t remainder)
{
    uint32_t result, first_mul2;
    // better swap when first is lesser. It multiplicating scans bits of second
    if (first < second)
    {
        result = second;
        second = first;
        first = result;
    }
    // initialization depend on second loweset bit
    result = second & 1 ? first  :0 , first_mul2 = first;
    // first_mul2 containes subsequent multiples  first  by 2. Multiplcation uses  second bits as usual factorization to sum of power 2 and uses sequently computed multiples  first  by 2
    while (second >>= 1)
    {
        first_mul2 = remainder_add32(first_mul2, first_mul2, remainder);
        if (second & 1)
            result = remainder_add32(result, first_mul2, remainder);
    } 

    return result;
}

uint32_t remainder_pow32(uint32_t first, uint32_t second, uint32_t remainder)
{    
    // initialization depend on second loweset bit
    uint32_t result = second & 1 ? first : 1, first_pow2 = first;
    // first_pow2 containes subsequent powers first  by 2. Power uses  second bits as usual factorization exponent second to sum of power 2 and uses sequently computed powers first  by 2
    while (second >>= 1)
    {
        first_pow2 = remainder_mul32(first_pow2, first_pow2, remainder);
        if (second & 1)
            result = remainder_mul32(result, first_pow2, remainder);
    }

    return result;
}

uint64_t remainder_add(uint64_t first, uint64_t second, uint64_t remainder)
{
    while (first + second < first)
    {
        first = (first + second) % remainder;
        second = UINT64_MAX % remainder + 1;
    }
    return (first + second) % remainder;
}

uint64_t remainder_mul(uint64_t first, uint64_t second, uint64_t remainder)
{
    uint64_t result, first_mul2;
    // better swap when first is lesser. It multiplicating scans bits of second
    if (first < second)
    {
        result = second;
        second = first;
        first = result;
    }
    // initialization depend on second low3eset bit
    result = second & 1 ? first : 0, first_mul2 = first;
    // first_mul2 containes subsequent multiples  first  by 2. Multiplcation uses  second bits as usual factorization to sum of power 2 and uses sequently computed multiples  first  by 2
    while (second >>= 1)
    {
        first_mul2 = remainder_add(first_mul2, first_mul2, remainder);
        if (second & 1)
            result = remainder_add(result, first_mul2, remainder);
    }

    return result;
}

void remainder_mul2(uint64_t matrix[2][2], uint64_t vector[2], uint64_t result[2], uint64_t remainder)
{
    uint64_t result_0 =
        remainder_add(
            remainder_mul(matrix[0][0], vector[0], remainder),
            remainder_mul(matrix[0][1], vector[1], remainder),
            remainder
        );

    uint64_t result_1 = remainder_add(
        remainder_mul(matrix[1][0], vector[0], remainder),
        remainder_mul(matrix[1][1], vector[1], remainder),
        remainder
    );
   
    result[0] = result_0;
    result[1] = result_1;
}

void remainder_mul4(uint64_t first[2][2], uint64_t second[2][2], uint64_t result[2][2], uint64_t remainder)
{
    uint64_t result_0[2] = { second[0][0], second[1][0] };
    remainder_mul2(first, result_0, result_0, remainder);

    uint64_t result_1[2] = { second[0][1], second[1][1] };
    remainder_mul2(first, result_1, result_1, remainder);
    
    result[0][0] = result_0[0];
    result[1][0] = result_0[1];
    result[0][1] = result_1[0];
    result[1][1] = result_1[1];
}

void assign_matrix_one(uint64_t matrix[2][2])
{
    matrix[0][0] = 1;
    matrix[0][1] = 0;
    matrix[1][0] = 0;
    matrix[1][1] = 1;   
}

void assign_matrix_fibonacci(uint64_t matrix[2][2])
{
    const uint64_t first = first_fibonacci_coefficient, second = second_fibonacci_coefficient;
    matrix[0][0] = second*second+ first;
    matrix[0][1] = second*first;
    matrix[1][0] = second;
    matrix[1][1] = first;
}

void assign_matrix_second(uint64_t matrix[2][2], uint64_t second[2][2], uint64_t remainder)
{
    matrix[0][0] = second[0][0] % remainder;
    matrix[0][1] = second[0][1] % remainder;
    matrix[1][0] = second[1][0] % remainder;
    matrix[1][1] = second[1][1] % remainder;
}

void remainder_pow4(uint64_t matrix[2][2], uint64_t power, uint64_t result[2][2], uint64_t remainder)
{
    // Matrix_pow2 containes subsequent powers of matrix by 2. Making power of matrix uses  similar approach as making power of number. 
    // Exponent bits as usual factorizes exponent to sum of power 2 which is matched subsequently powers of matrix by 2.
    uint64_t matrix_pow2[2][2];
    assign_matrix_second(matrix_pow2, matrix, remainder);

    // Initialization depend on power loweset bit - one is power one
    if (power & 1)
        assign_matrix_second(result, matrix, remainder);
    else
        assign_matrix_one(result);    
    
    // Scan power bits and subsequently multiply result by powers of matrix by 2.
    while (power >>= 1)
    {
        remainder_mul4(matrix_pow2, matrix_pow2, matrix_pow2, remainder);
        if (power & 1)
            remainder_mul4(result, matrix_pow2, result, remainder);
    }    
}

uint64_t fibonacci_remainder_by_power(uint64_t fibonacci_number, uint64_t fibonacci_remainder)
{    
    if (fibonacci_number == 0)
        return first_fibonacci_number;
    else if (fibonacci_number == 1)
        return second_fibonacci_number;

    uint64_t fibonaci_matrix[2][2];
    assign_matrix_fibonacci(fibonaci_matrix);
    remainder_pow4(fibonaci_matrix, fibonacci_number / 2, fibonaci_matrix, fibonacci_remainder);

    uint64_t fibonaci_vector[2] = { second_fibonacci_number, first_fibonacci_number }; 
    remainder_mul2(fibonaci_matrix, fibonaci_vector, fibonaci_vector, fibonacci_remainder);
    return fibonacci_number & 1 ? fibonaci_vector[0] : fibonaci_vector[1];
}

uint64_t fibonacci_remainder_by_cycle(uint64_t fibonacci_number, uint64_t fibonacci_remainder )
{
    if (fibonacci_number == 0)
        return first_fibonacci_number;
    else if (fibonacci_number == 1)
        return second_fibonacci_number;
    
    uint64_t fibonacci_first = first_fibonacci_number, fibonacci_second = second_fibonacci_number, fibonacci_current = 1, fibonacci_thirth;
    while (fibonacci_current < fibonacci_number)
    {    
        ++fibonacci_current;
        fibonacci_thirth = remainder_add(fibonacci_first, fibonacci_second, fibonacci_remainder);
        fibonacci_first = fibonacci_second;
        fibonacci_second = fibonacci_thirth;
        // cycle is found, cut down calculation
        if (fibonacci_first == first_fibonacci_number && fibonacci_second == second_fibonacci_number)
            return fibonacci_remainder_by_cycle(fibonacci_number % (fibonacci_current - 1), fibonacci_remainder);
    }
    return fibonacci_second;
}
