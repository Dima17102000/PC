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
	double sum_1 = 0.0;
	double step = 1.0 / iterations;
	int max_threads = 10;
	// std::vector<double> partial_sums(max_threads, 0.0);
#pragma omp parallel for num_threads(10) reduction(+ : sum_1)
	// int tid = omp_get_thread_num();
	// sleep(tid);
	// int n_threads = omp_get_num_threads();
	for (int i = 0; i < iterations; i++)
	{
		double x = (i + 0.5) * step;
		sum_1 += 4.0 / (1.0 + x * x);
	}
	double pi = sum_1 * step;
	std::cout << "pi: " << std::fixed << pi << std::endl;
}

long long factorial(int n)
{
	long long result = 1;
	for (int i = 1; i <= n; i++)
	{
		result *= i;
	}
	return result;
}

int main1()
{
	std::cout << factorial(5) << std::endl;
	return 0;
}

/*
17 0.0792457
18 0.0794172
19 0.0793311
20 0.0794399
21 0.0794921
22 0.0797068
23 0.0796479
24 0.07975
25 0.0798706
26 0.0802027
27 0.0812299
28 0.0802326
29 0.0808873
30 0.0804613
31 0.0804573
32 0.0802588
*/