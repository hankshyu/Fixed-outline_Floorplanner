#include <stdio.h>
int divisorCloseRoot(long long num);
int main(){
    for(long long i = 2499900000; i < 2500000000; i++){
        printf("[%lld]: %d x %d\n",i, divisorCloseRoot(i), i/divisorCloseRoot(i));
    }
}
int divisorCloseRoot(long long num){

    if (num < 4) return 1;
    else{
        int divisor = 1;
        for (long long i = 2; i < num; i++) {
            if (i * i == num) return i;
            if (i * i < num && num % i == 0) divisor = i;
            if (i * i > num) return divisor;
        }
    }


    return -1;
}