#include <stdio.h>
#include "calc_basic.h"
#include "calc_advance.h"

int main(int argc, char const *argv[])
{
    int a, b;
    printf("Input a, b: ");
    scanf("%d %d", &a, &b);

    printf("Basic operation: \n");
    printf("Add: %d + %d = %d\n", a, b, add(a, b));
    printf("Sub: %d - %d = %d\n", a, b, subtract(a, b));
    printf("Mul: %d x %d = %d\n", a, b, multiply(a, b));
    printf("Div: %d / %d = %.3f\n", a, b, divide(a, b));

    printf("\nAdvanced operations: \n");
    printf("Power: %d^%d = %d\n", a, b, power(a, b));
    printf("Factorial: %d! = %lld\n", a, factorial(a));

    return 0;
}
