#include <stdio.h>

int f(int x, int n) {
    n = x + 10;
    return x*x + n;
}


int main() {
    int n = 5;
    printf("Original n value: n = %d\n", n);
    int f_val = f(2, n);
    printf("Value of f(n): f(n) = %d\nNew value for n: n = %d\n", f_val, n);
    return 0;
}