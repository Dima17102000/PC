#include <chrono>
#include <iomanip>
#include <iostream>

int main()
{
    long num_steps = 10000000;

    auto ts_1 = std::chrono::high_resolution_clock::now();

    long i;
    double sum = 0.0, x = 0.0;
    double step = 1.0 / num_steps;

    for (i = 0; i < num_steps; i++)
    {
        x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    auto ts_2 = std::chrono::high_resolution_clock::now();

    std::cout << "pi: " << std::fixed << std::setprecision(4) << sum * step << ", execution time: " << (std::chrono::duration<double>(ts_2 - ts_1).count()) << std::endl;

    return 0;
}