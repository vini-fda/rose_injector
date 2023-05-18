#include <stdio.h>

static int n = 5;

int f(int x) {
    n = x + 10;
    return x*x + n;
}


int main() {
    printf("Original n value: n = %d\n", n);
    int f_val = f(2);
    printf("Value of f(n): f(n) = %d\nNew value for n: n = %d\n", f_val, n);
    return 0;
}