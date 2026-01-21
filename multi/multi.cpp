#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
 
__attribute__((noinline)) long func2() {
    const long total = 100000000L;
    volatile long sum = 0;
    for (long i = 0; i < total; ++i) {
        sum += i;
    }
    return sum;
}
 
__attribute__((noinline)) long func1(long start, long end) {
    volatile long sum = 0;
    for (long i = start; i < end; ++i) {
        sum += i;
    }
    return sum;
}
 
__attribute__((noinline)) long func1_parallel() {
    const long total = 400000000L;
    const int nthreads = 16;
    std::vector<long> partial_sums(nthreads, 0.0);
    std::vector<std::thread> threads;
    long per_thread = total / nthreads;
 
    for (int t = 0; t < nthreads; ++t) {
        long start = t * per_thread;
        long end = (t == nthreads - 1) ? total : start + per_thread;
        threads.emplace_back([&, t, start, end]() {
            partial_sums[t] = func1(start, end);
        });
    }
 
    for (auto& th : threads) {
        th.join();
    }
 
    long sum = 0.0;
    for (const auto& p : partial_sums) {
        sum += p;
    }
    return sum;
}
 
int main() {
    auto start1 = std::chrono::high_resolution_clock::now();
    long s1 = func1_parallel();
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration1 = end1 - start1;
    std::cout << "func1_parallel runtime: " << duration1.count() << " seconds" << std::endl;
    if (s1 != 200000000L * 399999999L) {
      std::cout << "WRONG RESULT\n";
      return -1;
    }
 
    auto start2 = std::chrono::high_resolution_clock::now();
    long s2 = func2();
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration2 = end2 - start2;
    std::cout << "func2 runtime: " << duration2.count() << " seconds" << std::endl;

    return s1 + s2;
}