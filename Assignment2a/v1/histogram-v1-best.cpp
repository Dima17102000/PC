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
    generator(const int& max) : max(max), dist(0, max-1) { }
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
        #pragma omp parallel
        {
            int local_hist[10] = {0};
            generator number_generator(bins);

            #pragma omp for
            for (int i = 0; i < sample_size; i++) {
                int random_number = number_generator();
                local_hist[random_number]++;
            }

            #pragma omp critical
            {
                for (int b = 0; b < bins; b++) {
                    data[b] += local_hist[b];
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