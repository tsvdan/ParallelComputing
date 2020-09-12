#pragma once
#include <cstddef>
#include <ostream>
#include <vector>

class Matrix {
private:
    size_t rows_;
    size_t columns_;
    std::vector<double> data_;
public:
    Matrix() = default;
    Matrix(size_t m, size_t n)
      : rows_(m), columns_(n), data_(m * n) {}
    Matrix(size_t m, size_t n, double value)
      : rows_(m), columns_(n), data_(m * n, value) {}
    Matrix(size_t m, size_t n, std::vector<double> values)
      : rows_(m), columns_(n), data_(values) {}
    Matrix(const Matrix& M);
    // рассказать Асе, почему у неё лучше не inlud'ить "Vector.h" сюда

    Matrix& operator=(const Matrix& M);

    Matrix T();
    double FrobeniusNorm() const;

    void operator+=(const Matrix& B);
    void operator-=(const Matrix& B);
    void operator*=(double multi);
    void operator*=(const Matrix& B);

    double& operator()(size_t i, size_t j) {
        return data_.at(i * columns_ + j);
    };

    double operator()(size_t i, size_t j) const {
        return data_.at(i * columns_ + j);
    };

    size_t rows() const { return rows_; }
    size_t columns() const { return columns_; }

    friend std::ostream& operator<<(std::ostream&, const Matrix& M);
};

Matrix operator*(const Matrix&, double);
Matrix operator*(double, const Matrix&);

Matrix operator+(const Matrix&, const Matrix&);
Matrix operator-(const Matrix&, const Matrix&);
Matrix operator*(const Matrix&, const Matrix&);
std::ostream& operator<<(std::ostream& os, const Matrix& A);

bool all_close(const Matrix& A, const Matrix& B, double eps=1e-7);

std::tuple<std::vector<size_t>*, Matrix*, Matrix*> PLU_decompose(const Matrix& A);

