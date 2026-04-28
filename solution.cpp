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


pair <int,int> process_range(int start,int end) {
    int loc_c1 = 0, loc_c2 = 0;
    for(int i = start; i < end; i++) {
        if(i % 2 == 0) {
            ++loc_c1;
        }
        else{
            ++loc_c2;
        }
    }
    return {loc_c1,loc_c2};
}

int main() {

    const int N = 1'000'000;
    const int num_tasks = thread::hardware_concurrency();
    vector<future<pair<int,int>>> futures;
    for(int i = 0; i < num_tasks; ++i) {
        int start = (N * i) / num_tasks;
        int end = (N * (i + 1) / num_tasks);
        futures.push_back(async(launch::async,process_range,start,end));
    }
    int counter1 = 0, counter2 = 0;
    for(auto& f : futures) {
        auto [c1,c2] = f.get();
        counter1 += c1;
        counter2 += c2;
    }
}

#include <iostream>
#include <utility>
#include <future>
#include <thread>
#include <vector>


using namespace std;

pair<int,int> process_range2(int start,int end) {
    int loc_1 = 0, loc_2 = 0;
    for(int i = start; i < end; i++) {

        if(i % 2 == 0){
            loc_1++;
        }
        else {
            loc_2++;
        }
    }
    return {loc_1,loc_2};
}


int main() {
    const int N = 1'000'000;
    const int num_tasks = thread::hardware_concurrency();
    vector<future<pair<int,int>>> futures;
    for(int i = 0; i < num_tasks; i++) {
        int start = (N * i) / num_tasks;
        int end = (N * (i + 1) / num_tasks);
        futures.push_back(async(launch::async,process_range2,start,end));
        int counter1 = 0, counter2 = 0;
        for(auto & f: futures) {
            auto[c1,c2] = f.get();
            counter1 += c1;
            counter2 += c2;
        }
    }
}