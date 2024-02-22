#ifndef FibonacciRemainders
#define FibonacciRemainders
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void assert(int value, const char* message);
uint32_t remainder_add32(uint32_t fibonacci_first, uint32_t fibonacci_second, uint32_t fibonacci_remainder);
uint32_t remainder_mul32(uint32_t fibonacci_first, uint32_t fibonacci_second, uint32_t fibonacci_remainder);
uint32_t remainder_pow32(uint32_t first, uint32_t second, uint32_t remainder);
void remainder_pow4(uint64_t matrix[2][2], uint64_t power, uint64_t result[2][2], uint64_t remainder);
uint64_t fibonacci_remainder_by_power(uint64_t fibonacci_number, uint64_t fibonacci_remainder);
uint64_t fibonacci_remainder_by_cycle(uint64_t fibonacci_number, uint64_t fibonacci_remainder);

#ifdef __cplusplus
}
#endif
#endif