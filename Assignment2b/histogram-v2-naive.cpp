#include <vector>
#include <iostream>
#include <chrono>
#include <random>
#include <omp.h>

struct generator
{
private:
	int bins;

public:
	generator(const int &max) : bins(max) {}
	int operator()(int n)
	{
		int num_factors = 0;
		for (int p = 2; p <= n; ++p)
		{
			while (n % p == 0)
			{
				n = n / p;
				num_factors++;
			}
		}
		return (num_factors < bins - 1) ? num_factors : bins - 1;
	}
};

struct histogram
{
	int bins, *data;
	histogram(int count) : bins(count)
	{
		data = (int *)malloc(sizeof(int) * count);
		for (int b = 0; b < count; b++)
			data[b] = 0;
	}
	~histogram() { free(data); }

	void populate(int sample_size)
	{
		generator number_generator(bins);
#pragma omp parallel for reduction(+ : data[ : 10]) firstprivate(number_generator) schedule(guided)
		for (int i = 2; i < sample_size; ++i)
		{
			int bin = number_generator(i);
			data[bin]++;
		}
	}

	void print()
	{
		int total = 0;
		for (int b = 0; b < bins; ++b)
		{
			total += data[b];
			std::cout << b << ":" << data[b] << std::endl;
		}
		std::cout << "total: " << total << std::endl;
	}
};

int main(int argc, char **argv)
{
	int num_bins = 10;
	int sample_ceiling = 50000;
	std::cout << "Bins: " << num_bins << ", sample ceiling: " << sample_ceiling << std::endl;

	histogram h(num_bins);
	auto t1 = std::chrono::high_resolution_clock::now();
	h.populate(sample_ceiling);
	auto t2 = std::chrono::high_resolution_clock::now();

	h.print();
	std::cout << "\ntime elapsed: " << std::chrono::duration<double>(t2 - t1).count() << " seconds." << std::endl;
	return 0;
}