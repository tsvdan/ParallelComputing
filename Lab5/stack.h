#pragma once
#include "vector.h"

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
        data_.pop_back();
    }

private:
    SafeVector<T> data_;
};