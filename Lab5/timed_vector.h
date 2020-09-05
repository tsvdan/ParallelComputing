#include "vector.h"
#include <chrono>
#include <iostream>

using std::cout;
using std::endl;

template<class T>
class TimedVector {
public:
    SafeVector<T> safe_vector;

    TimedVector(const SafeVector<T>& sv) : safe_vector(sv) {
        time_spawned = clock.now();
    }
    // другие конструкторы
    T operator[](size_t i) const {
        return safe_vector[i];
    }
    T& operator[](size_t i) {
        return safe_vector[i];
    }
    size_t size() { return safe_vector.size(); };

    // По уничтожению расскажем:
    // 1) минимальное, максимальное, среднее время ожидания
    // 2) эффективное использование (лок-время / время параллельного выполнения*)
    //      *следует создать timed_vector инстанс прямо перед началом параллельного участка
    ~TimedVector() {
        cout << "2. Коэф. эффективного использования: " << time_locked / (clock.now() - time_spawned) << endl;
    }
// Mutex'ы блокируются в этих функциях в оригинальном векторе,
// поэтому их и будем измерять
    std::chrono::time_point<std::chrono::steady_clock> push_back(const T& val) {
        auto time_start = clock.now();
        safe_vector.push_back(val);
        time_locked += clock.now() - time_start;
        return time_start;
    }
    std::chrono::time_point<std::chrono::steady_clock> pop_back() {
        auto time_start = clock.now();
        safe_vector.pop_back();
        time_locked += clock.now() - time_start;
        return time_start;
    }
    std::ostream& write_out(std::ostream& os) {
    // для вывода не буду замерять время ожидания
        auto time_start = clock.now();
        std::ostream& result = safe_vector.write_out(os);
        time_locked += clock.now() - time_start;
        return result;
    }
//

private:
    std::chrono::steady_clock clock;
    std::chrono::time_point<std::chrono::steady_clock> time_spawned{ clock.now() };
    std::chrono::duration<double> time_locked{ 0 };
};

template<typename T>
std::ostream& operator<<(std::ostream& os, TimedVector<T>& tv) {
    return tv.write_out(os);
}