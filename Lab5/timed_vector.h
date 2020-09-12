#pragma once
#include "vector.h"
#include <cstddef>
#include <iostream>
#include <mutex>
#include <chrono>

typedef std::mutex mutex_type;
typedef std::lock_guard<mutex_type> lock_type;
typedef std::chrono::time_point<std::chrono::steady_clock> time_point;

template<class T>
class TimedVector {
public:
    TimedVector() {
        size_ = 0;
        capacity_ = 2;
        data_ = new T[capacity_];
    }
    TimedVector(size_t s) {
        capacity_ = (s != 0 ? 2 * s : 2);
        size_ = s;
        data_ = new T[capacity_];
    };
    TimedVector(size_t s, const T& const_val) {
        capacity_ = (s != 0 ? 2 * s : 2);
        size_ = s;
        data_ = new T[capacity_];
        for (size_t i = 0; i < s; ++i) {
            data_[i] = const_val;
        }
    };
    TimedVector(const SafeVector<T>& sv) {
        capacity_ = sv.size() * 2;
        size_ = sv.size();
        data_ = new T[capacity_];
        for (size_t i = 0; i < sv.size(); ++i) {
            data_[i] = sv[i];
        }
    };

    TimedVector& operator=(TimedVector rhs) {
        this->capacity_ = rhs.capacity_;
        this->size_ = rhs.size_;
        std::swap(this->data_, rhs.data_);
        return (*this);
    }


    ~TimedVector() {
        delete[] data_;
        std::cout << "2. Коэф. эффективного использования: " << time_locked / (clock.now() - time_spawned) << std::endl;
    };

    T& operator[](size_t i) {
        if (i >= size_) {
            throw "Invalid index";
        }
        lock_type lock(m);
        return data_[i];
    }

    T operator[](size_t i) const {
        if (i >= size_) {
            throw "Invalid index";
        }
        return data_[i];
    }

    time_point push_back(const T& val) {
        lock_type lock(m);
        auto time_acquired_lock = clock.now();
        if (size_ == capacity_) {
            resize_();
        }
        data_[size_] = val;
        ++size_;
        time_locked += clock.now() - time_acquired_lock;
        return time_acquired_lock;
    }

    time_point pop_back() {
        lock_type lock(m);
        auto time_acquired_lock = clock.now();
        if (!empty()) {
            --size_;
        }
        time_locked += clock.now() - time_acquired_lock;
        return time_acquired_lock;
    }

    bool empty() const {
        return size_ == 0;
    }

    size_t size() const {
        return size_;
    }

    std::ostream& write_out(std::ostream& os) {
        lock_type lock(m);
        auto time_acquired_lock = clock.now();
        for (size_t i = 0; i < size_; ++i) {
            os << data_[i] << ' ';
        }
        time_locked += clock.now() - time_acquired_lock;
        return os;
    }  // should be private and have a friend

private:
    size_t size_;
    size_t capacity_;
    T* data_;
    mutex_type m;

    std::chrono::steady_clock clock;
    std::chrono::duration<double> time_locked{ 0 };
    time_point time_spawned{ clock.now() };

    void resize_() {
        auto tmp_data = new T[2 * capacity_];
        for (size_t i = 0; i < size_; ++i) {
            tmp_data[i] = data_[i];
        }
        delete[] data_;
        data_ = tmp_data;
        capacity_ *= 2;
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, TimedVector<T>& tv) {
    return tv.write_out(os);
}