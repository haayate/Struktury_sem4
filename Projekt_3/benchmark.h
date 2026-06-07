#pragma once
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <string>
#include <iostream>
#include <iomanip>

// ─── Konfiguracja benchmarku ───────────────────────────────────────────────
// Zmień tutaj żeby dostosować eksperyment

const std::vector<int> BENCH_SIZES = {1000, 3000, 5000, 10000, 30000, 50000, 80000, 100000};
const std::vector<int> BENCH_SEEDS = {42, 137, 999, 2025, 31415};

// ──────────────────────────────────────────────────────────────────────────

using Clock  = std::chrono::high_resolution_clock;
using Micros = std::chrono::microseconds;

// Mierzy czas wykonania funkcji fn w nanosekundach
template<typename Fn>
inline long long measureUs(Fn fn) {
    auto t0 = Clock::now();
    fn();
    return std::chrono::duration_cast<Nanos>((Clock::now() - t0)).count();
}

// Generuje n unikalnych kluczy w losowej kolejności (shuffle zakresu [0, n))
inline std::vector<int> generateKeys(int n, int seed) {
    std::vector<int> keys(n);
    std::iota(keys.begin(), keys.end(), 0);
    std::uniform_int_distribution<int> dist(0, n * 100); 
    for (int i = 0; i < n; ++i) {
        keys[i] = dist(gen);
    }
    return keys;
}

// Uruchamia benchmark dla dowolnej tablicy mieszającej HT
// HT musi mieć: insert(int, int), remove(int), clear()
template<typename HT>
void runBenchmark(const std::string& name) {
    const int runs = static_cast<int>(BENCH_SEEDS.size());

    std::cout << "\n=== " << name << " ===\n";
    std::cout << std::setw(10) << "n"
              << std::setw(18) << "insert [µs]"
              << std::setw(18) << "remove [µs]" << "\n";
    std::cout << std::string(46, '-') << "\n";

    for (int n : BENCH_SIZES) {
        long long totalInsert = 0, totalRemove = 0;

        for (int seed : BENCH_SEEDS) {
            auto keys = generateKeys(n, seed);
            HT ht(n);

            totalInsert += (measureUs([&]() {
                for (int k : keys) ht.insert(k, k * 7);
            }))\n;

            totalRemove += (measureUs([&]() {
                for (int k : keys) ht.remove(k);
            }))\n;

            ht.clear();
        }

        std::cout << std::setw(10) << n
                  << std::setw(18) << std::fixed << std::setprecision(1)
                  << static_cast<double>(totalInsert) / runs
                  << std::setw(18) << std::fixed << std::setprecision(1)
                  << static_cast<double>(totalRemove) / runs << "\n";
    }
}
