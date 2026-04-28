#include <vector>
#include <iostream>
#include <chrono>
#include <random>
#include <omp.h>

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
        for (int b = 0; b < count; ++b) data[b] = 0;
    }

    ~histogram() { free(data); }

    void populate(int sample_size) {
        const int BLOCK = 1000;

        #pragma omp parallel
        {
            generator number_generator(bins);
            int local_counts[10] = {0};   // bins = 10, фиксированный размер
            int local_iter = 0;            // счётчик итераций в этом потоке

            #pragma omp for
            for (int i = 0; i < sample_size; ++i) {
                int r = number_generator();
                local_counts[r]++;
                local_iter++;

                if (local_iter == BLOCK) {
                    for (int b = 0; b < bins; ++b) {
                        if (local_counts[b] != 0) {
                            #pragma omp atomic
                            data[b] += local_counts[b];
                            local_counts[b] = 0;
                        }
                    }
                    local_iter = 0;
                }
            }

            // Остатки
            for (int b = 0; b < bins; ++b) {
                if (local_counts[b] != 0) {
                    #pragma omp atomic
                    data[b] += local_counts[b];
                }
            }
        }
    }

    void print() {
        int total = 0;
        for (int b = 0; b < bins; ++b) {
            cout << b << ": " << data[b] << endl;
            total += data[b];
        }
        cout << "total: " << total << endl;
    }
};

int main() {
    int num_bins = 10;
    int sample_count = 30000000;
    cout << "Bins: " << num_bins << ", sample size: " << sample_count << endl;

    histogram h(num_bins);
    auto t1 = chrono::high_resolution_clock::now();
    h.populate(sample_count);
    auto t2 = chrono::high_resolution_clock::now();

    h.print();
    cout << "\ntime elapsed: " << chrono::duration<double>(t2 - t1).count() << " seconds." << endl;
}