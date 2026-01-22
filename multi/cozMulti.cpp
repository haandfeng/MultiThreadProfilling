#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include "coz.h" 

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
    const int iters = 100;
    long acc = 0;

    for (int i = 0; i < iters; i++) {
        // auto start1 = std::chrono::high_resolution_clock::now();
        long s1 = func1_parallel();
        // COZ_PROGRESS_NAMED("func1_parallel_done"); 
        long s2 = func2();
        acc += (s1 + s2);
        COZ_PROGRESS_NAMED("everything_done");

    }

    return acc;
}