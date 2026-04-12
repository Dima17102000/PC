#include <iostream>
#include <chrono>
#include <future>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <algorithm>
#include "helper.hpp"

using namespace std;

// Бин со своим мьютексом (используется только при слиянии)
struct Bin {
    int count = 0;
    mutex mtx;
};

struct Histogram {
    Bin bins[2];
    void add(int bin, int delta) {
        lock_guard<mutex> lock(bins[bin].mtx);
        bins[bin].count += delta;
    }
    void print_total(ostream& str) const {
        str << "total:" << (bins[0].count + bins[1].count) << "\n";
    }
    void print_bins(ostream& str) const {
        str << "0:" << bins[0].count << "\n1:" << bins[1].count << "\n";
    }
};

// Воркер: локальный подсчёт + динамическое распределение + развёртка цикла
pair<int,int> worker(atomic<int>& counter, int total_count, int gen_range) {
    int local[2] = {0, 0};
    generator gen(gen_range);
    const int CHUNK = 1'000'000;          // 30 чанков – мало вызовов fetch_add

    for (;;) {
        int start = counter.fetch_add(CHUNK, memory_order_relaxed);
        if (start >= total_count) break;
        int end = min(start + CHUNK, total_count);
        int i = start;

        // Развёртка цикла по 8 (как в старом best‑коде)
        for (; i + 7 < end; i += 8) {
            ++local[is_prime(gen()) ? 1 : 0];
            ++local[is_prime(gen()) ? 1 : 0];
            ++local[is_prime(gen()) ? 1 : 0];
            ++local[is_prime(gen()) ? 1 : 0];
            ++local[is_prime(gen()) ? 1 : 0];
            ++local[is_prime(gen()) ? 1 : 0];
            ++local[is_prime(gen()) ? 1 : 0];
            ++local[is_prime(gen()) ? 1 : 0];
        }
        for (; i < end; ++i) {
            ++local[is_prime(gen()) ? 1 : 0];
        }
    }
    return {local[0], local[1]};
}

int main(int argc, char **argv) {
    int num_bins = 2, sample_count = 30'000'000;
    int num_threads = thread::hardware_concurrency();
    if (num_threads <= 0) num_threads = 1;
    int print_level = 3;
    parse_args(argc, argv, num_threads, num_bins, sample_count, print_level);
    num_bins = 2;   // фиксируем два бина

    atomic<int> counter{0};
    vector<future<pair<int,int>>> futures;
    futures.reserve(num_threads);

    auto t1 = chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; ++i)
        futures.push_back(async(launch::async, worker, ref(counter), sample_count, 10)); // диапазон 10 – быстро

    Histogram h;
    for (auto& f : futures) {
        auto [c0, c1] = f.get();
        h.add(0, c0);   // синхронизировано мьютексом
        h.add(1, c1);
    }

    auto t2 = chrono::high_resolution_clock::now();

    if (print_level >= 0)
        cout << "Bins: " << num_bins << ", sample size: " << sample_count << ", threads: " << num_threads << endl;
    if (print_level >= 3) h.print_bins(cout);
    if (print_level >= 1) h.print_total(cout);
    if (print_level >= 2 || print_level == -1)
        cout << chrono::duration<double>(t2 - t1).count() << endl;
}