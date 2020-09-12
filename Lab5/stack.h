#pragma once
#include "vector.h"
#include <ostream>

template<class T>
class SafeStack {
public:
    SafeStack() {}

    void push(const T& val) {
        data_.push_back(val);
    }

    T& peek() {
        if (data_.empty()) {
            throw "Peeking empty stack";
        }
        return data_[data_.size() - 1];
    }

    void pop() {
        // noexcept?
        data_.pop_back();
    }

    void identity() {
        data_.m.lock();
        
        if (data_.size_ == data_.capacity_) {
            data_.resize_();
        }

        data_.data_[data_.size_] = T();
        ++data_.size_;
        --data_.size_;

        data_.m.unlock();
    }

    std::ostream& write_out(std::ostream& os) {
        for (size_t i = 0; i < data_.size(); ++i) {
            os << data_[i] << " ";
        }
        return os << (!data_.empty() ? "<-- голова" : "Стэк пуст");
    }

    size_t size() {
        return data_.size();
    }

private:
    SafeVector<T> data_;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, SafeStack<T>& st) {
    return st.write_out(os);
}