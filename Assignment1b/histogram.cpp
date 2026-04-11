#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <numeric>
#include <thread>
#include <atomic>
#include <mutex>
#include "helper.hpp"

using namespace std;

struct Bucket {
	int count = 0;
	std::mutex mtx;

	void add (int delta){
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

	int& get(int i)	{
		return bins[i].count;
	}

	void print_total(std::ostream& str) {
		int total = 0;
		for(auto& bin: bins) total += bin.count;
		str << "total:" << total << "\n";
	}

	void print_bins(std::ostream& str) {
		for (size_t i = 0; i < bins.size(); ++i) str << i << ":" << bins[i].count << "\n";
	}
};

void worker(std::atomic<int>& counter, int sample_count, histogram& h, int gen_range) 
{
    generator gen(gen_range);
    const int CHUNK = 10000;

    for (;;) {
        int start = counter.fetch_add(CHUNK, std::memory_order_relaxed);
        if (start >= sample_count) break;

        int end = std::min(start + CHUNK, sample_count);
        int local_counts[2] = {0, 0};

        for (int i = start; i < end; ++i) {
            int value = gen();
            int bin = is_prime(value) ? 1 : 0;
            local_counts[bin]++;
        }

        if (local_counts[0] > 0) h.bins[0].add(local_counts[0]);
        if (local_counts[1] > 0) h.bins[1].add(local_counts[1]);
    }
}

int main(int argc, char **argv)
{
	int num_bins = 2;
	int sample_count = 30000000;

	int num_threads = 1;//std::thread::hardware_concurrency();
	int print_level = 3; // 0: exec info + histogram total, 1: + histogram bins, 2: +exec time, 3: +bin info
	parse_args(argc, argv, num_threads, num_bins, sample_count, print_level);
	num_bins = 2;

	histogram h(num_bins);
	std::atomic<int> counter{0};
	std::vector<std::thread> threads;

	auto t1 = chrono::high_resolution_clock::now();

	for(int i = 0; i < num_threads; i++) {
		threads.emplace_back(worker,std::ref(counter),sample_count,std::ref(h),10);
	}
	
	for(auto& t: threads) {
		t.join();
	}

	auto t2 = chrono::high_resolution_clock::now();

	if ( print_level >= 0 ) cout << "Bins: " << num_bins << ", sample size: " << sample_count << ", threads: " << num_threads << endl;
	if ( print_level >= 3 ) h.print_bins(cout);
	if ( print_level >= 1 ) h.print_total(cout);
	if ( print_level >= 2 || print_level == -1 ) cout << chrono::duration<double>(t2 - t1).count() << endl;
}