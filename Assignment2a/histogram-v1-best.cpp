// #include <vector>
// #include <iostream>
// #include <chrono>
// #include <random>
// #include <omp.h>

// using namespace std;

// struct generator {
// private:
//     int max;
//     std::minstd_rand engine;
//     std::uniform_int_distribution<int> dist;

// public:
//     generator(const int& max) : dist(0, max-1) { }
//     int operator()() {
//         return dist(engine);
//     }
// };

// struct histogram {
//     int bins, *data;

//     histogram(int count) : bins(count) {
//         data = (int*) malloc(sizeof(int) * count);
//         for (int b = 0; b < count; ++b) data[b] = 0;
//     }

//     ~histogram() { free(data); }

//     void populate(int sample_size) {
//         // Узнаём число потоков (через environment)
//         int nthreads = 1;
//         #pragma omp parallel
//         {
//             #pragma omp single
//             nthreads = omp_get_num_threads();
//         }

//         // Выделяем один большой блок памяти для всех локальных гистограмм
//         int* local_data = new int[nthreads * bins]();
//         // эквивалент calloc: все нули

//         #pragma omp parallel
//         {
//             int tid = omp_get_thread_num();
//             generator number_generator(bins);
//             int* mydata = local_data + tid * bins;   // указатель на локальную гистограмму

//             #pragma omp for
//             for (int i = 0; i < sample_size; ++i) {
//                 int r = number_generator();
//                 mydata[r]++;    // нет синхронизации, каждый поток пишет в свою память
//             }
//         }

//         // Слияние локальных гистограмм в общую (последовательно, без синхронизации)
//         for (int b = 0; b < bins; ++b) {
//             int sum = 0;
//             for (int t = 0; t < nthreads; ++t) {
//                 sum += local_data[t * bins + b];
//             }
//             data[b] = sum;
//         }

//         delete[] local_data;
//     }

//     void print() {
//         int total = 0;
//         for (int b = 0; b < bins; ++b) {
//             cout << b << ": " << data[b] << endl;
//             total += data[b];
//         }
//         cout << "total: " << total << endl;
//     }
// };

// int main() {
//     int num_bins = 10;
//     int sample_count = 30000000;
//     cout << "Bins: " << num_bins << ", sample size: " << sample_count << endl;

//     histogram h(num_bins);
//     auto t1 = chrono::high_resolution_clock::now();
//     h.populate(sample_count);
//     auto t2 = chrono::high_resolution_clock::now();

//     h.print();
//     cout << "\ntime elapsed: " << chrono::duration<double>(t2 - t1).count() << " seconds." << endl;
// }

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
        #pragma omp parallel
        {
            int local[10] = {0};          // локальная гистограмма на стеке
            generator gen(bins);          // каждый поток создаёт свой генератор

            #pragma omp for
            for (int i = 0; i < sample_size; ++i) {
                local[gen()]++;
            }

            // Один раз после цикла добавляем локальные данные в глобальную гистограмму
            #pragma omp critical
            for (int b = 0; b < bins; ++b) {
                data[b] += local[b];
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