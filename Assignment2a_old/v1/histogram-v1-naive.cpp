#include <vector>
#include <iostream>
#include <chrono>
#include <random>

using namespace std;

struct generator {
private:
    int max;
    std::minstd_rand engine; 
    std::uniform_int_distribution<int> dist;

public:
    generator(const int& max) : dist(0, max-1) { }
    int operator()() {
        return dist(engine);
    }
};

struct histogram {
    int bins, *data;

    histogram(int count) : bins(count) {
        data = (int*) malloc(sizeof(int) * count);
        for (int b = 0; b < count; b++) {
            data[b] = 0;
        }
    }

    ~histogram() { free(data); }

    void populate(int sample_size) {
        // NAIVE PARALLEL VERSION with critical section inside the loop
        #pragma omp parallel
        {
            // Each thread needs its own generator (private by default)
            generator number_generator(bins);
            
            // Distribute loop iterations among threads
            #pragma omp for
            for (int i = 0; i < sample_size; i++) {
                int random_number = number_generator();
                
                // Critical section: only one thread can update the histogram at a time
                #pragma omp critical
                {
                    data[random_number]++;
                }
            }
        }
    }

    void print() {
        int total = 0;
        for (int b = 0; b < bins; b++) {
            std::cout << b << ": " << data[b] << std::endl;
            total += data[b];
        }
        std::cout << "total: " << total << std::endl;
    }
};

int main(int argc, char **argv) {
    int num_bins = 10;
    int sample_count = 30000000;

    std::cout << "Bins: " << num_bins << ", sample size: " << sample_count << std::endl;
    
    histogram h(num_bins);

    auto t1 = chrono::high_resolution_clock::now();
    h.populate(sample_count);
    auto t2 = chrono::high_resolution_clock::now();

    h.print();
    std::cout << "\ntime elapsed: " << chrono::duration<double>(t2 - t1).count() << " seconds." << std::endl;
    
    return 0;
}