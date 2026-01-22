#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
// #include "coz.h" 

__attribute__((noinline)) long func2(long total) {
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
 
__attribute__((noinline)) long func1_parallel(long total) {
    const int nthreads = 16;
    std::vector<long> partial_sums(nthreads, 0);
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
 
    long sum = 0;
    for (const auto& p : partial_sums) {
        sum += p;
    }
    return sum;
}
 
int main() {
    const int iters = 100;
    const long func1_base_total = 400000000L;
    const long func2_base_total = 100000000L;
    const long func2_0pct_total = func2_base_total * 0 / 100;
    const long func2_60pct_total = func2_base_total * 40 / 100;
    const long func2_95pct_total = func2_base_total * 5 / 100;
    const long func1_0pct_total = func1_base_total * 0 / 100;
    const long func1_40pct_total = func1_base_total * 60 / 100;
    struct Scenario {
        const char* name;
        long func1_total;
        long func2_total;
    };
    std::vector<Scenario> scenarios = {
        {"baseline", func1_base_total, func2_base_total},
        {"func2 -60%", func1_base_total, func2_60pct_total},
        {"func2 -95%", func1_base_total, func2_95pct_total},
        {"func1 -40%", func1_40pct_total, func2_base_total},
        {"Only func2", func1_0pct_total, func2_base_total},
        {"Only func1", func1_base_total, func2_0pct_total},
        

    };
    long acc = 0;

    long long baseline_ms = 0;
    bool has_baseline = false;
    for (const auto& scenario : scenarios) {
        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < iters; i++) {
            long s1 = func1_parallel(scenario.func1_total);
            long s2 = func2(scenario.func2_total);
            acc += (s1 + s2);
        }
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << scenario.name << " time: " << ms << " ms";
        if (!has_baseline) {
            baseline_ms = ms;
            has_baseline = true;
            std::cout << std::endl;
            continue;
        }
        long long saved = baseline_ms - ms;
        double saved_pct = baseline_ms > 0 ? (static_cast<double>(saved) * 100.0 / baseline_ms) : 0.0;
        std::cout << " (saved " << saved << " ms, " << saved_pct << "%)" << std::endl;
    }

    return acc;
}
