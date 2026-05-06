// #include <iostream>
// #include <vector>
// #include <thread>
// #include <future>

// using namespace std;

// pair <int,int> process_range(int start,int end) {
//     int loc_counter1 = 0, loc_counter2 = 0;
//     for(int i = start; i < end; ++i) {
//         if(i % 2 == 0) {
//             ++loc_counter1;
//         }
//         else {
//             ++loc_counter2;
//         }
//     }
//     return {loc_counter1,loc_counter2};
// }

// int main() {

//     const int N = 1000000;
//     const int num_tasks = thread::hardware_concurrency();
//     vector<future<pair<int,int>>> futures;

//     for(int i = 0; i < num_tasks; ++i) {
//         int from = (N * i) / num_tasks;
//         int to = (N * (i + 1)) / num_tasks;
//         futures.push_back(async(launch::async,process_range,from,to));
//     }

//     int counter1 = 0, counter2 = 0;

//     for(auto& f : futures) {
//         auto [c1, c2] = f.get();
//         counter1 += c1;
//         counter2 += c2;
//     }

//     cout << "Counter1 = " << counter1 << endl;
//     cout << "Counter2 = " << counter2 << endl;
// }

// #include <iostream>
// #include <vector>
// #include <future>
// #include <thread>
// #include <utility>

// using namespace std;

// pair<int,int> process_working(int start,int end) {
//     int local_count1 = 0, local_count2 = 0;
//     for(int i = start; i < end; ++i) {
//         if(i % 2 == 0)
//         {
//             ++local_count1;
//         }
//         else
//         {
//             ++local_count2;
//         }
//     }
//     return {local_count1,local_count2};
// }

// int main() {
//     const int N = 1'000'000;
//     const int num_tasks = thread::hardware_concurrency();
//     vector<future<pair<int,int>>> futures;

//     for(int i = 0; i < num_tasks; ++i) {
//         int start = (N * i) / num_tasks;
//         int end = (N * (i + 1)) / num_tasks;
//         futures.push_back(async(launch::async,process_working,start,end));
//     }

//     int counter1 = 0,counter2 = 0;

//     for(auto& f: futures) {
//         auto[c1,c2] = f.get();
//         counter1 += c1;
//         counter2 += c2;
//     }

//     cout << "Counter1 = " << counter1 << endl;
//     cout << "Counter2 = " << counter2 << endl;
// }

#include <iostream>
#include <vector>
#include <future>
#include <thread>
#include <utility>

using namespace std;

pair<int, int> process_range(int start, int end)
{
    int loc_c1 = 0, loc_c2 = 0;
    for (int i = start; i < end; i++)
    {
        if (i % 2 == 0)
        {
            ++loc_c1;
        }
        else
        {
            ++loc_c2;
        }
    }
    return {loc_c1, loc_c2};
}

int main()
{

    const int N = 1'000'000;
    const int num_tasks = thread::hardware_concurrency();
    vector<future<pair<int, int>>> futures;
    for (int i = 0; i < num_tasks; ++i)
    {
        int start = (N * i) / num_tasks;
        int end = (N * (i + 1)) / num_tasks;
        futures.push_back(async(launch::async, process_range, start, end));
    }
    int counter1 = 0, counter2 = 0;
    for (auto &f : futures)
    {
        auto [c1, c2] = f.get();
        counter1 += c1;
        counter2 += c2;
    }
}

#include <iostream>
#include <vector>
#include <future>
#include <thread>

using namespace std;

// // Эту функцию НЕ меняем
// void work(int start, int end, vector<int>& counters) {
//     for (int i = start; i < end; ++i) {
//         if (i % 2 == 0)
//             counters[0]++;
//         else
//             counters[1]++;
//     }
// }

// int main() {
//     const int N = 1'000'000;

//     int num_tasks = thread::hardware_concurrency();

//     vector<future<vector<int>>> futures;

//     for (int i = 0; i < num_tasks; ++i) {
//         int start = (N * i) / num_tasks;
//         int end   = (N * (i + 1)) / num_tasks;

//         futures.push_back(async(launch::async, [start, end]() {
//             vector<int> local_counters(2, 0);

//             work(start, end, local_counters);

//             return local_counters;
//         }));
//     }

//     vector<int> counters(2, 0);

//     for (auto& f : futures) {
//         vector<int> local = f.get();

//         counters[0] += local[0];
//         counters[1] += local[1];
//     }

//     cout << "counter 1 = " << counters[0] << endl;
//     cout << "counter 2 = " << counters[1] << endl;

//     return 0;
// }

// #include <iostream>
// #include <vector>
// #include <future>
// #include <thread>
// #include <mutex>

// using namespace std;

// vector <int> counters(2,0);
// mutex m;
// void work(int start,int end)
// {
//     for(int i = start; i < end; i++)
//     {
//         lock_guard<mutex> lock(m);
//         if(i % 2 == 0) {
//             counters[0]++;
//         }
//         else {
//             counters[1]++;
//         }
//     }
// }

// int main() {
//     const int N = 1'000'000;
//     const int num_tasks = thread::hardware_concurrency();
//     vector<future<void>> futures;
//     for(int i = 0; i < num_tasks; i++)
//     {
//         int start = (N * i) / num_tasks;
//         int end = (N * (i+1)) / num_tasks;
//         futures.push_back(async(launch::async,work,start,end));
//     }
//     for(auto& f: futures) {
//         f.get();
//     }
// }

// #include <iostream>
// #include <vector>
// #include <thread>
// #include <mutex>
// #include <future>

// using namespace std;

// void work(int start, int end,vector<int>& v1)
// {
//     for(int i = start; i < end; i++) {
//         if(i % 2 == 0) {
//             v1[0]++;
//         }
//         else{
//             v1[1]++;
//         }
//     }
// }

// int main() {

//     const int N = 1'000'000;
//     int num_tasks = thread::hardware_concurrency();
//     vector<future<vector<int>>> futures;
//     for(int i = 0; i < num_tasks; i++) {
//         int start = (N * i) / num_tasks;
//         int end = (N * (i + 1)) / num_tasks;
//         futures.push_back(async(launch::async,[start,end]()
//         {
//             vector<int> counters(2,0);
//             work(start,end,counters);
//             return counters;
//         }));
//     }
//     vector<int> local(2,0);

//     for(auto & f: futures) {
//         vector<int> local_counter = f.get();
//         local[0] += local_counter[0];
//         local[1] += local_counter[1];
//     }
// }

// #include <iostream>
// #include <thread>
// #include <vector>
// #include <mutex>
// #include <utility>

// using namespace std;

// void worker(int start, int end, vector<int> &v1)
// {
//     for (int i = start; i < end; i++)
//     {
//         if (i % 2 == 0)
//         {
//             v1[0]++;
//         }
//         else
//         {
//             v1[1]++;
//         }
//     }
// }

// int main()
// {
//     const int N = 1'000'000;
//     int num_tasks = thread::hardware_concurrency();
//     vector<future<vector<int>>> futures;
//     for (int i = 0; i < num_tasks; i++)
//     {
//         int start = (N * i) / num_tasks;
//         int end = (N * (i + 1)) / num_tasks;
//         futures.push_back(async(launch::async, [start, end]
//                                 {
//             vector<int>counter(2,0);
//             worker(start,end,counter);
//             return counter; }));
//     }
//     vector<int> local(2, 0);
//     for (auto &f : futures)
//     {
//         vector<int> loc_counter = f.get();
//         local[0] += loc_counter[0];
//         local[1] += loc_counter[1];
//     }
// }

// #include <iostream>
// #include <vector>
// #include <thread>
// #include <future>
// #include <utility>
// #include <mutex>

// using namespace std;

// vector<int> counters;
// mutex m;
// void worker1(int start, int end)
// {
//     for (int i = start; i < end; i++)
//     {
//         lock_guard<mutex> lock(m);
//         if (i % 2 == 0)
//         {
//             counters[0]++;
//         }
//         else
//         {
//             counters[1]++;
//         }
//     }
// }

// int main()
// {
//     const int N = 1'000'000;
//     int num_tasks = thread::hardware_concurrency();
//     vector<future<void>> futures;
//     for (int i = 0; i < num_tasks; i++)
//     {
//         int start = (N * i) / num_tasks;
//         int end = (N * (i + 1)) / num_tasks;
//         futures.push_back(async(launch::async, worker1, start, end));
//     }
//     for (auto &f : futures)
//     {
//         f.get();
//     }
// }