#include <vector>
#include <iostream>
#include <chrono>
#include <random>

struct generator
{
private:
	int bins;

public:
	generator(const int &max) : bins(max) {}
	int operator()(int n)
	{
		int num_factors = 0;

		// Start with 2 and increment by 1 to check each number
		for (int p = 2; p <= n; ++p)
		{
			// While i divides n, print i and divide n
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
		// allocate memory for histogram
		data = (int *)malloc(sizeof(int) * count);

		// initialize histogram with 0's
		for (int b = 0; b < count; b++)
		{
			data[b] = 0;
		}
	}

	~histogram() { free(data); }

	void populate(int sample_size)
	{
		// initialize prime factors generator
		generator number_generator(bins);

		for (int i = 2; i < sample_size; i++)
		{
			// count number of prime factors for integer i
			int number_of_primes = number_generator(i);

			// update corresponding bin
			data[number_of_primes]++;
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

	// initialize and empty histogram with 'num_bins' bins
	histogram h(num_bins);

	auto t1 = std::chrono::high_resolution_clock::now();

	// populate the histogram that was just created
	h.populate(sample_ceiling);

	auto t2 = std::chrono::high_resolution_clock::now();

	// print the contents of the histogram and the time it took populate it
	h.print();

	std::cout << "\ntime elapsed: " << std::chrono::duration<double>(t2 - t1).count() << " seconds." << std::endl;
}

// #include <iostream>
// #include <chrono>
// #include <omp.h>

// struct generator
// {
// private:
//     int bins;

// public:
//     generator(const int &max) : bins(max) {}
//     // int operator()(int n)
//     // {
//     // 	int num_factors = 0;
//     // 	for (int p = 2; p <= n; ++p)
//     // 	{
//     // 		while (n % p == 0)
//     // 		{
//     // 			n /= p;
//     // 			++num_factors;
//     // 		}
//     // 	}
//     // 	return (num_factors < bins - 1) ? num_factors : bins - 1;
//     // }
//     int operator()(int n)
//     {
//         int num_factors = 0;

//         for (int p = 2; p <= n; ++p)
//         {
//             while (n % p == 0)
//             {
//                 n /= p;
//                 ++num_factors;
//             }
//         }

//         return (num_factors < bins - 1) ? num_factors : bins - 1;
//     }
// };

// struct histogram
// {
//     int bins, *data;
//     histogram(int count) : bins(count)
//     {
//         data = new int[count]();
//     }
//     ~histogram() { delete[] data; }

//     void populate(int sample_size)
//     {
//         int nthreads = omp_get_max_threads();
//         const int padding = 16;                               // 16 int = 64 байта (cache line)
//         int *per_thread_data = new int[nthreads * padding](); // обнулён

// #pragma omp parallel
//         {
//             int tid = omp_get_thread_num();
//             generator g(bins);
//             int *local = per_thread_data + tid * padding; // каждый поток на своей cache line
// #pragma omp for schedule(guided, 4)
//             for (int i = 2; i < sample_size; ++i)
//             {
//                 int bin = g(i);
//                 ++local[bin];
//             }
//         }

//         // Суммирование (только реальные bins, игнорируем padding)
//         for (int t = 0; t < nthreads; ++t)
//         {
//             int *local = per_thread_data + t * padding;
//             for (int b = 0; b < bins; ++b)
//             {
//                 data[b] += local[b];
//             }
//         }
//         delete[] per_thread_data;
//     }

//     void print()
//     {
//         int total = 0;
//         for (int b = 0; b < bins; ++b)
//         {
//             total += data[b];
//             std::cout << b << ":" << data[b] << '\n';
//         }
//         std::cout << "total: " << total << std::endl;
//     }
// };

// int main()
// {
//     const int num_bins = 10;
//     const int sample_ceiling = 50000;
//     std::cout << "Bins: " << num_bins << ", sample ceiling: " << sample_ceiling << std::endl;
//     histogram h(num_bins);
//     auto t1 = std::chrono::high_resolution_clock::now();
//     h.populate(sample_ceiling);
//     auto t2 = std::chrono::high_resolution_clock::now();
//     h.print();
//     std::cout << "\ntime elapsed: " << std::chrono::duration<double>(t2 - t1).count() << " seconds." << std::endl;
//     return 0;
// }

// //version2

// #include <vector>
// #include <iostream>
// #include <chrono>
// #include <random>
// #include <omp.h>
// struct generator
// {
// private:
//     int bins;

// public:
//     generator(const int &max) : bins(max) {}
//     int operator()(int n)
//     {
//         int num_factors = 0;

//         // Start with 2 and increment by 1 to check each number
//         for (int p = 2; p <= n; ++p)
//         {
//             // While i divides n, print i and divide n
//             while (n % p == 0)
//             {
//                 n = n / p;
//                 num_factors++;
//             }
//         }
//         return (num_factors < bins - 1) ? num_factors : bins - 1;
//     }
// };

// struct histogram
// {
//     int bins, *data;

//     histogram(int count) : bins(count)
//     {
//         // allocate memory for histogram
//         data = (int *)malloc(sizeof(int) * count);

//         // initialize histogram with 0's
//         for (int b = 0; b < count; b++)
//         {
//             data[b] = 0;
//         }
//     }

//     ~histogram() { free(data); }

//     void populate(int sample_size)
//     {
//         int b0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 = 0;
//         int b5 = 0, b6 = 0, b7 = 0, b8 = 0, b9 = 0;

//         generator number_generator(bins);
// #pragma omp parallel for default(none) shared(sample_size, bins) firstprivate(number_generator) reduction(+ : b0, b1, b2, b3, b4, b5, b6, b7, b8, b9) schedule(static, 1)
//         for (int i = 2; i < sample_size; i++)
//         {
//             int bin = number_generator(i);

//             switch (bin)
//             {
//             case 0:
//                 b0++;
//                 break;
//             case 1:
//                 b1++;
//                 break;
//             case 2:
//                 b2++;
//                 break;
//             case 3:
//                 b3++;
//                 break;
//             case 4:
//                 b4++;
//                 break;
//             case 5:
//                 b5++;
//                 break;
//             case 6:
//                 b6++;
//                 break;
//             case 7:
//                 b7++;
//                 break;
//             case 8:
//                 b8++;
//                 break;
//             default:
//                 b9++;
//                 break;
//             }
//         }

//         data[0] = b0;
//         data[1] = b1;
//         data[2] = b2;
//         data[3] = b3;
//         data[4] = b4;
//         data[5] = b5;
//         data[6] = b6;
//         data[7] = b7;
//         data[8] = b8;
//         data[9] = b9;
//     }

//     void print()
//     {
//         int total = 0;
//         for (int b = 0; b < bins; ++b)
//         {
//             total += data[b];
//             std::cout << b << ":" << data[b] << std::endl;
//         }
//         std::cout << "total: " << total << std::endl;
//     }
// };

// int main(int argc, char **argv)
// {
//     int num_bins = 10;
//     int sample_ceiling = 50000;

//     std::cout << "Bins: " << num_bins << ", sample ceiling: " << sample_ceiling << std::endl;

//     // initialize and empty histogram with 'num_bins' bins
//     histogram h(num_bins);

//     auto t1 = std::chrono::high_resolution_clock::now();

//     // populate the histogram that was just created
//     h.populate(sample_ceiling);

//     auto t2 = std::chrono::high_resolution_clock::now();

//     // print the contents of the histogram and the time it took populate it
//     h.print();

//     std::cout << "\ntime elapsed: " << std::chrono::duration<double>(t2 - t1).count() << " seconds." << std::endl;
// }