#include "vector.h"
#include "stack.h"
#include "queue.h"
#include "timed_vector.h"
#include <iostream>
#include <thread>
#include <chrono>

using std::cout;
using std::endl;

const int NUM_EXTRA_THREADS = 6;  // где-то ближе к концу используется

int main() {
    SafeVector<double> sv(9, 0);
    cout << sv.size() << endl;
    for (int i = 0; i < sv.size(); ++i) {
        sv[i] = i*i;
    }
    cout << sv << endl;

    std::vector<std::chrono::duration<double>> waiting_times; 
    waiting_times.reserve(NUM_EXTRA_THREADS * 10 * 2 + 10);          // :))))))))))))

    TimedVector<double> tv(sv);
    // это синтакс для лямбд: auto название_функции = [захват внешних переменных] (параметры) -> return_type { тело функции; };
    auto lambda_1 = [&] () -> void { 
        for (int i = 0; i < 10; ++i) {
            // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            auto time_try = std::chrono::steady_clock::now();
            auto time_locked = tv.push_back(i);
            waiting_times.push_back(time_locked - time_try);
        }
    };
    auto lambda2_10 = [&] () -> void {
        for (int i = 0; i < 10; ++i) {
            // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            auto time_try = std::chrono::steady_clock::now();
            auto time_locked = tv.push_back(100);  // если бы я блокировал mutex на обе операции, а не по разу на каждую из них, то они бы действительно ничего не делали
            waiting_times.push_back(time_locked - time_try);
            time_try = std::chrono::steady_clock::now();
            time_locked = tv.pop_back();     //  а так получается иногда неожиданно, но благо не крашится ничего, size_ ожидаемый
            waiting_times.push_back(time_locked - time_try);
        }                     //   а это фактически корректное поведение
    };
    auto lambda_print = [&] () -> void { 
        for (int i = 0; i < 21; ++i) {
            // std::this_thread::sleep_for(std::chrono::milliseconds(51));
            cout << tv << endl;
        }
    };
    std::thread t1(lambda_1);
    std::vector<std::thread> ths(NUM_EXTRA_THREADS);
    for (int i = 0; i < NUM_EXTRA_THREADS; ++i) {
        ths.at(i) = std::thread(lambda2_10);
    }
    std::thread t_print(lambda_print);
    t1.join();
    for (int i = 0; i < NUM_EXTRA_THREADS; ++i) {
        ths.at(i).join();
    }
    t_print.join(); // ожидал 0 1 4 9 16 25 36 49 64 0 1 2 3 4 5 6 7 8 9, но вообще говоря это некорректный тест
    cout << tv << endl;

    // здесь вызовется ~TimedVector, в котором я рассказываю про коэф эффективного использования (и думал рассказывать про время ожидания)
    // коэф низкий только из-за std::this_thread::sleep_for -- закомментировать и ок

    double mean_waiting_time{ 0 };
    double min_waiting_time{ 1 };  // 1. sec
    double max_waiting_time{ 0 };
    for (auto wt : waiting_times) {
        double duration = wt.count();
        mean_waiting_time += duration;
        if (duration < min_waiting_time) { min_waiting_time = duration; }
        if (duration > max_waiting_time) { max_waiting_time = duration; }
    }
    mean_waiting_time /= waiting_times.size();
    cout << "1. Среднее время ожидания разделяемого ресурса: " << mean_waiting_time << endl;
    cout << "   минимальное время ожидания:\t"  << min_waiting_time << endl;
    cout << "   максимальное время ожидания:\t" << max_waiting_time << endl;
    
}


// Пока у меня:
// 1) плохой тест
// 2) коэф эф пользования иногда получается > 1
// 3) нет стэка
// 4) нет кью










