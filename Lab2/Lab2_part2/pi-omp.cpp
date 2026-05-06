// #include <chrono>
// #include <iomanip>
// #include <iostream>

// int main()
// {
//     long num_steps = 10000000;

//     auto ts_1 = std::chrono::high_resolution_clock::now();

//     long i;
//     double sum = 0.0, x = 0.0;
//     double step = 1.0 / num_steps;

//     for (i = 0; i < num_steps; i++)
//     {
//         x = (i + 0.5) * step;
//         sum += 4.0 / (1.0 + x * x);
//     }

//     auto ts_2 = std::chrono::high_resolution_clock::now();

//     std::cout << "pi: " << std::fixed << std::setprecision(4) << sum * step << ", execution time: " << (std::chrono::duration<double>(ts_2 - ts_1).count()) << std::endl;

//     return 0;
// }

#include <chrono>
#include <iomanip>
#include <iostream>
#include <omp.h>

int main()
{
    long num_steps = 10000000;

    auto ts_1 = std::chrono::high_resolution_clock::now();

    double sum = 0.0;
    double step = 1.0 / num_steps;

#pragma omp parallel default(none) shared(num_steps, step, sum)
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        double local_sum = 0.0;
        double x = 0.0;

        for (long i = thread_id; i < num_steps; i += num_threads)
        {
            x = (i + 0.5) * step;
            local_sum += 4.0 / (1.0 + x * x);
        }

#pragma omp critical
        {
            sum += local_sum;
        }
    }

    auto ts_2 = std::chrono::high_resolution_clock::now();

    std::cout << "pi: " << std::fixed << std::setprecision(4)
              << sum * step
              << ", execution time: "
              << std::chrono::duration<double>(ts_2 - ts_1).count()
              << std::endl;

    return 0;
}