#pragma once
#include <cstddef>
#include <iostream> //<ostream>
#include <mutex>

typedef std::mutex mutex_type;
typedef std::lock_guard<mutex_type> lock_type;

template<class T>
class SafeVector {
public:
    SafeVector() {
    // Constructs a vector of size 0
    // Assumes T::T()
        size_ = 0;
        capacity_ = 2;
        data_ = new T[capacity_];
    }
    SafeVector(size_t s) {
    // Constructs a vector of size s
    // Default initialized -- assumes T::T()
        capacity_ = (s != 0 ? 2 * s : 2);
        size_ = s;
        data_ = new T[capacity_];
    };
    SafeVector(size_t s, const T& const_val) {
    // Constructs a vector of size s
    // Every value equals const_val -- assumes T::operator=
        capacity_ = (s != 0 ? 2 * s : 2);
        size_ = s;
        data_ = new T[capacity_];
        for (size_t i = 0; i < s; ++i) {
            data_[i] = const_val;
        }
    };
    SafeVector(const SafeVector<T>& sv) {
        capacity_ = sv.capacity_;
        size_ = sv.size_;
        data_ = new T[capacity_];
        for (size_t i = 0; i < sv.size_; ++i) {
            data_[i] = sv.data_[i];
        }
    };

    SafeVector& operator=(SafeVector rhs) {
        this->capacity_ = rhs.capacity_;
        this->size_ = rhs.size_;
        std::swap(this->data_, rhs.data_);
        return (*this);
    }


    ~SafeVector() {
        std::cout << "size=" << size_ << "\tcapacity=" << capacity_ << std::endl;
        delete[] data_;
        size_ = 0;
    };

    T& operator[](size_t i) {
        if (i >= size_) {
            throw "Invalid index";
        }
        return data_[i];
    }

    T operator[](size_t i) const {
        if (i >= size_) {
            throw "Invalid index";
        }
        return data_[i];
    }

    void push_back(const T& val) {
        lock_type lock(m);
        if (size_ == capacity_) {
            resize_();
        }
        data_[size_] = val;
        ++size_;
    }

    void pop_back() {
        lock_type lock(m);
        if (!empty()) {
            --size_;
        }
    }

    bool empty() const {
        return size_ == 0;
    }

    size_t size() const {
        return size_;
    }

    std::ostream& write_out(std::ostream& os) {
        lock_type lock(m);
        for (size_t i = 0; i < size_; ++i) {
            os << data_[i] << ' ';
        }
        return os;
    }  // should be private and have a friend

private:
    size_t size_;
    size_t capacity_;
    T* data_;
    mutex_type m;

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
std::ostream& operator<<(std::ostream& os, SafeVector<T>& sv) {
    return sv.write_out(os);
}