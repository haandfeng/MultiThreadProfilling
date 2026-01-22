#include <iostream>
#include <chrono>
#include "coz.h"
__attribute__((noinline)) long func2() {
    const long total = 100000000L;
    volatile long sum = 0;
    for (long i = 0; i < total; ++i) {
        sum += i;
    }
    return sum;
}
 
__attribute__((noinline)) long func1() {
    const long total = 400000000L;
    volatile long sum = 0;
    for (long i = 0; i < total; ++i) {
        sum += i;
    }
    return sum;
}
 
int main() {
    long s1 = func1();
 
    long s2 = func2();

    COZ_PROGRESS_NAMED("everything_done");
    return s1 + s2;
}

