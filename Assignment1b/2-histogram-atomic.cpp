#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <numeric>
#include <thread>
#include <atomic>
#include <cstring>

#include "helper.hpp"

using namespace std;

const int MAX_RANDOM = 1000000;
static vector<bool> prime_table;

void build_prime_table(int limit) {
    prime_table.assign(limit, true);
    if (limit > 0) prime_table[0] = false;
    if (limit > 1) prime_table[1] = false;
    for (int i = 2; i * i < limit; ++i) {
        if (prime_table[i]) {
            for (int j = i * i; j < limit; j += i)
                prime_table[j] = false;
        }
    }
}

inline bool is_prime_fast(int n) {
    return n >= 0 && n < (int)prime_table.size() && prime_table[n];
}


struct alignas(64) AtomicBucket {
    std::atomic<int> count;
    char padding[64 - sizeof(std::atomic<int>)];
    
    AtomicBucket() : count(0) {}
    
    void add(int delta) {
        count.fetch_add(delta, std::memory_order_relaxed);
    }
};

struct histogram {
    std::vector<AtomicBucket> bins;
    
    histogram(int count) : bins(count) { }

    void add(int i) {
        bins[i].add(1);
    }
    
    void add_batch(int i, int delta) {
        bins[i].add(delta);
    }

    int get(int i) const {
        return bins[i].count.load(std::memory_order_relaxed);
    }

    void print_total(std::ostream& str) {
        int total = 0;
        for (const auto& bin : bins) {
            total += bin.count.load(std::memory_order_relaxed);
        }
        str << "total:" << total << "\n";
    }

    void print_bins(std::ostream& str) {
        for (size_t i = 0; i < bins.size(); ++i) {
            str << i << ":" << bins[i].count.load(std::memory_order_relaxed) << "\n";
        }
    }
};

void worker(int total_samples, histogram& h, std::atomic<int>& next_index, int max_random) {
  
    generator gen(max_random);
    
    
    int local_counts[2] = {0, 0};
    const int BATCH_SIZE = 20000;   
    
    while (true) {
       
        int start = next_index.fetch_add(BATCH_SIZE, std::memory_order_relaxed);
        if (start >= total_samples) break;
        int end = min(start + BATCH_SIZE, total_samples);
        
        
        for (int i = start; i < end; ++i) {
            int val = gen();                
            int bin = is_prime_fast(val) ? 0 : 1;
            local_counts[bin]++;
        }
        
        
        for (int bin = 0; bin < 2; ++bin) {
            if (local_counts[bin] > 0) {
                h.add_batch(bin, local_counts[bin]);
                local_counts[bin] = 0;
            }
        }
    }
}

int main(int argc, char **argv) {
    int num_bins = 2;
    int sample_count = 30000000;
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads <= 0) num_threads = 1;
    int print_level = 3;

    const int MAX_RANDOM = 1000000;
    parse_args(argc, argv, num_threads, num_bins, sample_count, print_level);
    
    num_bins = 2;  
    
    histogram h(num_bins);
    std::atomic<int> next_index(0);
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    

    auto t1 = chrono::high_resolution_clock::now();
    
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, sample_count, std::ref(h), std::ref(next_index), MAX_RANDOM);
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


