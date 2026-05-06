// #include <iostream>
// int main()
// {
// 	int iterations = 100000000;
// 	double sum = 0.0;
// 	double step = 1.0 / iterations;
// 	for (int i = 0; i < iterations; i++)
// 	{
// 		double x = (i + 0.5) * step;
// 		sum += 4.0 / (1.0 + x * x);
// 	}
// 	std::cout << "pi: " << std::fixed << sum * step << std::endl;
// }

#include <iostream>
#include <omp.h>
#include <unistd.h>
#include <vector>
int main()
{
	int iterations = 100000000;
	double sum = 0.0;
	double step = 1.0 / iterations;
	int max_threads = 10;
	std::vector<double> partial_sums(max_threads, 0.0);
#pragma omp parallel num_threads(10)
	{
		int tid = omp_get_thread_num();
		// sleep(tid);
		// int n_threads = omp_get_num_threads();
#pragma omp for
		for (int i = 0; i < iterations; i++)
		{
			double x = (i + 0.5) * step;
			partial_sums[tid] += 4.0 / (1.0 + x * x);
		}
	}
	for (int i = 0; i < max_threads; i++)
	{
		sum += partial_sums[i];
	}
	double pi = sum * step;
	std::cout << "pi: " << std::fixed << pi << std::endl;
}
