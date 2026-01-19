#include <iostream>
#include <chrono>
 
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
    auto start1 = std::chrono::high_resolution_clock::now();
    long s1 = func1();
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration1 = end1 - start1;
    std::cout << "func1 runtime: " << duration1.count() << " seconds" << std::endl;
 
    auto start2 = std::chrono::high_resolution_clock::now();
    long s2 = func2();
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration2 = end2 - start2;
    std::cout << "func2 runtime: " << duration2.count() << " seconds" << std::endl;
 
    return s1 + s2;
}

