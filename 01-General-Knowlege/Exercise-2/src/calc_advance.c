#include "calc_advance.h"

int power(int base, int exp){
    int ret = 1;
    
    for(int i = 0; i < exp; i++){
        ret *= base;
    }
    
    return ret;
}

long long factorial(int n){
    if(n == 0 || n == 1){
        return 1;
    }
    
    return n * factorial(n - 1);
}