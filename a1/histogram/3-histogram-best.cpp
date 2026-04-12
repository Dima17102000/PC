#include <iostream>
#include <chrono>
#include <future>
#include <vector>
#include <thread>

#include "helper.hpp"

using namespace std;


struct histogram {
    int data[10]{0};
    void print_total(std::ostream& str) const {
        int total = 0;
        for (int i = 0; i < 10; ++i) total += data[i];
        str << "total:" << total << "\n";
    }

    void print_bins(std::ostream& str) const {
        for (int i = 0; i < 10; ++i) {
            str << i << ":" << data[i] << "\n";
        }
    }
};


inline histogram worker(int sample_count)
{
    histogram local;
    generator gen(10);

    
    int n8 = sample_count / 8;
    int rem = sample_count % 8;

    while (n8--) {
        ++local.data[gen()];
        ++local.data[gen()];
        ++local.data[gen()];
        ++local.data[gen()];
        ++local.data[gen()];
        ++local.data[gen()];
        ++local.data[gen()];
        ++local.data[gen()];
    }

    while (rem--) {
        ++local.data[gen()];
    }

    return local;
}

int main(int argc, char **argv)
{
    int num_bins = 10;
    int sample_count = 30000000;

    int num_threads = std::thread::hardware_concurrency();
    if (num_threads <= 0) num_threads = 1;

    int print_level = 3;
    parse_args(argc, argv, num_threads, num_bins, sample_count, print_level);

    if (num_threads <= 0) num_threads = 1;

    histogram h;
    vector<future<histogram>> futures;
    futures.reserve(num_threads);

    auto t1 = chrono::high_resolution_clock::now();

    const int base = sample_count / num_threads;
    const int rem  = sample_count % num_threads;

    
    for (int t = 0; t < num_threads; ++t) {
        const int local_count = base + (t < rem ? 1 : 0);
        futures.push_back(std::async(std::launch::async, worker, local_count));
    }

    for (auto& f : futures) {
        histogram local = f.get();  
        
        h.data[0] += local.data[0];
        h.data[1] += local.data[1];
        h.data[2] += local.data[2];
        h.data[3] += local.data[3];
        h.data[4] += local.data[4];
        h.data[5] += local.data[5];
        h.data[6] += local.data[6];
        h.data[7] += local.data[7];
        h.data[8] += local.data[8];
        h.data[9] += local.data[9];
    }

    auto t2 = chrono::high_resolution_clock::now();

    if (print_level >= 0) {
        cout << "Bins: " << num_bins
             << ", sample size: " << sample_count
             << ", threads: " << num_threads << endl;
    }
    if (print_level >= 3) h.print_bins(cout);
    if (print_level >= 1) h.print_total(cout);
    if (print_level >= 2 || print_level == -1) {
        cout << chrono::duration<double>(t2 - t1).count() << endl;
    }
}