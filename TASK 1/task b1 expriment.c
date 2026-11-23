#include <stdio.h>

long long calls = 0;

int sum_digits(int n) {
    if (n == 0) return 0;
    return (n % 10) + sum_digits(n / 10);
}

int digital_root(int n) {
    if (n < 10) return n;
    return digital_root(sum_digits(n));
}

long long power_linear(long long base, int exp) {
    calls++;
    if (exp == 0) return 1;
    return base * power_linear(base, exp - 1);
}

long long power_fast(long long base, int exp) {
    calls++;
    if (exp == 0) return 1;
    long long half = power_fast(base, exp / 2);
    return (exp % 2 == 0) ? (half * half) : (base * half * half);
}

long long power_iterative(long long base, int exp) {
    long long result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) result *= base;
        base *= base;
        exp /= 2;
    }
    return result;
}

int main(void) {
    // B1.1: Digital Root Validation
    int test_ids[] = {0, 7, 9875, 123456};
    printf("--- Digital Root Tests ---\n");
    for (int i = 0; i < 4; i++) {
        printf("digital_root(%d) = %d\n", test_ids[i], digital_root(test_ids[i]));
    }

    // B1.3: Call Count Comparison
    int ns[] = {10, 100, 1000, 10000};
    printf("\n--- Call Count Comparison ---\n");
    printf("%-10s | %-15s | %-15s\n", "n", "Linear Calls", "Fast Calls");
    for (int i = 0; i < 4; i++) {
        calls = 0;
        power_linear(1, ns[i]);
        long long lin_calls = calls;

        calls = 0;
        power_fast(1, ns[i]);
        long long fast_calls = calls;

        printf("%-10d | %-15lld | %-15lld\n", ns[i], lin_calls, fast_calls);
    }

    // B1.4: Stack Overflow Limit Test (Uncomment to test crash threshold)
    // Typically crashes around n = 170,000 to 260,000 on default 8MB Linux stack
    /*
    printf("\n--- Testing Stack Limit ---\n");
    for (int n = 50000; n <= 1000000; n += 50000) {
        printf("Testing n = %d...\n", n);
        fflush(stdout);
        power_linear(1, n);
    }
    */
    printf("\nIterative power(2, 30) = %lld (No stack limit)\n", power_iterative(2, 30));
    return 0;
}
