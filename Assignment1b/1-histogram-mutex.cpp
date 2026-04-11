#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <numeric>
#include <thread>
#include <mutex>
#include <cstring>

#include "helper.hpp"

using namespace std;

// Простая структура без лишних сложностей
// alignas(64) гарантирует, что объекты Bucket будут на разных кэш-линиях
struct alignas(64) Bucket {
    int count;
    std::mutex mtx;
    
    Bucket() : count(0) {}
    
    void add(int delta) {
        std::lock_guard<std::mutex> lock(mtx);
        count += delta;
    }
};

struct histogram {
    std::vector<Bucket> bins;
    
    histogram(int count) : bins(count) { }

    void add(int i) {
        bins[i].add(1);
    }
    
    void add_batch(int i, int delta) {
        bins[i].add(delta);
    }

    int get(int i) const {
        return bins[i].count;
    }

    void print_total(std::ostream& str) {
        int total = 0;
        for (const auto& bin : bins) {
            total += bin.count;
        }
        str << "total:" << total << "\n";
    }

    void print_bins(std::ostream& str) {
        for (size_t i = 0; i < bins.size(); ++i) {
            str << i << ":" << bins[i].count << "\n";
        }
    }
};

void worker(int sample_count, histogram& h, int num_bins) {
    generator gen(num_bins);
    
    int local_counts[10] = {0};
    const int BATCH_SIZE = 1000;
    
    int processed = 0;
    while (processed < sample_count) {
        int batch_end = min(processed + BATCH_SIZE, sample_count);
        for (int i = processed; i < batch_end; ++i) {
            int next = gen();
            local_counts[next]++;
        }
        
        for (int bin = 0; bin < num_bins; ++bin) {
            if (local_counts[bin] > 0) {
                h.add_batch(bin, local_counts[bin]);
                local_counts[bin] = 0;
            }
        }
        
        processed = batch_end;
    }
}

int main(int argc, char **argv) {
    int num_bins = 10;
    int sample_count = 30000000;
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads <= 0) num_threads = 1;
    int print_level = 3;
    
    parse_args(argc, argv, num_threads, num_bins, sample_count, print_level);
    
    histogram h(num_bins);
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    
    auto t1 = chrono::high_resolution_clock::now();
    
    int base = sample_count / num_threads;
    int rem = sample_count % num_threads;
    
    for (int i = 0; i < num_threads; ++i) {
        int local_count = base + (i < rem ? 1 : 0);
        threads.emplace_back(worker, local_count, std::ref(h), num_bins);
    }
    
    for (auto& th : threads) {
        th.join();
    }
    
    auto t2 = chrono::high_resolution_clock::now();
    
    if (print_level >= 0) cout << "Bins: " << num_bins << ", sample size: " << sample_count << ", threads: " << num_threads << endl;
    if (print_level >= 3) h.print_bins(cout);
    if (print_level >= 1) h.print_total(cout);
    if (print_level >= 2 || print_level == -1) cout << chrono::duration<double>(t2 - t1).count() << endl;
}