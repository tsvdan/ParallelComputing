#pragma once
#include <queue>
#include <mutex>

template<class T>
class SafeQ {
public:
    SafeQ() {}
    ~SafeQ() {}

    void push(T val) {
        std::lock_guard<std::mutex> lock(m);
        q.push(val);
    }

    T& front() {
        std::lock_guard<std::mutex> lock(m);
        if (q.empty()) {
            throw "Empty queue";
        }
        return q.front();
    }

    bool pop() {
        std::lock_guard<std::mutex> lock(m);
        if (q.empty()) {
            return false;
        }
        q.pop();
        return true;
    }

//private:
    std::queue<T> q;
    std::mutex m;
};