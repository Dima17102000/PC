/*
TODO:
Parallelize the program as much as possible. Do not use thread. Use
future/async.
*/

#include <iostream>
#include <numeric>
#include <vector>
#include <future>

using namespace std;
struct statistics {
  long divisible_by_7 = 0;
  long divisible_by_11 = 0;
};

void worker(statistics& s, long from, long to) {
  for (long i = from; i < to; ++i) {
    if (i % 7 == 0) {
      ++s.divisible_by_7;
    } else if (i % 11) {
      ++s.divisible_by_11;
    }
  }
}

int main() {
  int sample_count = 1000000000;
  int thread_count = 4;  //  std::thread::hardware_concurrency();
  vector<future<statistics>> futures;
  statistics stats;
  long range_per_thread = sample_count / thread_count;

  for (int t = 0; t < thread_count; ++t) {
    long from = t * range_per_thread;
    long to = (t == thread_count - 1) ? sample_count : (t + 1) * range_per_thread;
    futures.push_back(async(launch::async,[from,to]{
      statistics local_stats;
      worker(local_stats,from,to);
      return local_stats;
    }));
  }
  for(auto& f : futures) {
    statistics local = f.get();
    stats.divisible_by_7 += local.divisible_by_7;
    stats.divisible_by_11 += local.divisible_by_11;
  }

  std::cout << "Divisible by 7:  " << stats.divisible_by_7 << std::endl;
  std::cout << "Divisible by 11: " << stats.divisible_by_11 << std::endl;

  return 0;
}