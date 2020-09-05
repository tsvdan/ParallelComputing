#include "vector.h"
#include "stack.h"
#include "queue.h"
#include "timed_vector.h"
#include <iostream>
#include <thread>
#include <chrono>

using std::cout;
using std::endl;


int main() {
    SafeVector<double> sv(9, 0);
    cout << sv.size() << endl;
    for (int i = 0; i < sv.size(); ++i) {
        sv[i] = i*i;
    }
    cout << sv << endl;

    // это синтакс для лямбд: auto название_функции = [захват внешних переменных] (параметры) -> return_type { тело функции; };
    auto lambda_1 = [&] () -> void { 
        for (int i = 0; i < 10; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            sv.push_back(i);
        }
    };
    auto lambda2_10 = [&] () -> void {
        for (int i = 0; i < 10; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            sv.push_back(100);
            sv.pop_back();
        }
    };
    auto lambda_print = [&] () -> void { 
        for (int i = 0; i < 21; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(51));
            cout << sv << endl;
        }
    };
    std::thread t1(lambda_1);
    const int NUM_EXTRA_THREADS = 10;
    std::vector<std::thread> ths(NUM_EXTRA_THREADS);
    for (int i = 0; i < NUM_EXTRA_THREADS; ++i) {
        ths.at(i) = std::thread(lambda2_10);
    }
    std::thread t_print(lambda_print);
    t1.join();
    for (int i = 0; i < NUM_EXTRA_THREADS; ++i) {
        ths.at(i).join();
    }
    t_print.join();
}
