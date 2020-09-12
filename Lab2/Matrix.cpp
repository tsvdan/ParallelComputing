#include "Matrix.h"
#include <omp.h>
#include <cmath>
#include <tuple>
#include <ostream>
#include <iomanip>

Matrix::Matrix(const Matrix& M)
    : rows_(M.rows_), columns_(M.columns_), data_(M.data_) {
}

Matrix& Matrix::operator=(const Matrix& M) {
    rows_ = M.rows_;
    columns_ = M.columns_;
    data_ = std::vector<double>(M.data_);
    return (*this);
}

bool is_dim_ok_additive(const Matrix& A, const Matrix& B) {
    return A.rows() == B.rows() && A.columns() == B.columns();
}

bool is_dim_ok_multiplicative(const Matrix& A, const Matrix& B) {
    return A.columns() == B.rows();
}

void Matrix::operator+=(const Matrix& B) {
    if (!is_dim_ok_additive(*this, B)) throw ("Invalid dimensions");
    #pragma omp parallel for
    for (size_t i = 0; i < rows(); ++i) {
        for (size_t j = 0; j < columns(); ++j) {
            (*this)(i, j) += B(i, j);
        }
    }
}

void Matrix::operator-=(const Matrix& B) {
    if (!is_dim_ok_additive(*this, B)) throw ("Invalid dimensions");
    #pragma omp parallel for
    for (size_t i = 0; i < B.rows(); ++i) {
        for (size_t j = 0; j < B.columns(); ++j) {
            (*this)(i, j) -= B(i, j);
        }
    }
} 

void Matrix::operator*=(double multi) {
    #pragma omp parallel for
    for (size_t i = 0; i < rows(); ++i) {
        for (size_t j = 0; j < columns(); ++j) {
            (*this)(i, j) *= multi;
        }
    }
}


void Matrix::operator*=(const Matrix& B) {
    if (!is_dim_ok_multiplicative(*this, B)) throw ("Invalid dimensions");
    Matrix C(rows(), B.columns());
    #pragma omp parallel for
    for (size_t i = 0; i < rows(); ++i) {
        for (size_t j = 0; j < B.columns(); ++j) {
            for (size_t k = 0; k < columns(); ++k) {
                C(i, j) += (*this)(i, k) * B(k, j);
            }
        }
    }
    (*this) = C;
}

Matrix operator*(const Matrix& A, const Matrix& B) {
    Matrix C(A);
    C *= B;
    return C;
}

Matrix operator+(const Matrix& A, const Matrix& B) {
    Matrix C(A);
    C += B;
    return C;
}

Matrix operator-(const Matrix& A, const Matrix& B) {
    Matrix C(A);
    C -= B;
    return C;
}

Matrix operator*(double multi, const Matrix& A) {
    Matrix C(A);
    C *= multi;
    return C;
}

Matrix operator*(const Matrix& A, double multi) {
    return multi * A;
}


std::ostream& operator<<(std::ostream& os, const Matrix& A) {
    os << std::fixed << std::setprecision(2);
    for (size_t i = 0; i < A.rows(); ++i) {
        for (size_t j = 0; j < A.columns(); ++j) {
            os << std::setw(10) << std::left << A(i, j);
        }
        os << '\n';
    }
    return os;
}

Matrix Matrix::T() {
    Matrix transposed(rows(), columns());
    #pragma omp parallel for
    for (size_t i = 0; i < rows(); ++i) {
        for (size_t j = 0; j < columns(); ++j) {
            transposed(i, j) = (*this)(j, i);
        }
    }
    return transposed;
}

double Matrix::FrobeniusNorm() const {
    double result = 0;
    #pragma omp parallel for reduction (+: result)
    for (size_t i = 0; i < rows(); ++i) {
        for (size_t j = 0; j < columns(); ++j) {
            result += pow((*this)(i, j), 2);
        }
    }
    return pow(result, 0.5);
}

bool all_close(const Matrix& A, const Matrix& B, double eps) {
    return (A - B).FrobeniusNorm() < eps;
}

std::tuple<std::vector<size_t>*, Matrix*, Matrix*> PLU_decompose(const Matrix& A) {
    if (!(A.rows() == A.columns())) {
        throw ("Only implemented for square matricies.");
    }
    size_t n = A.rows();

    // Permutation matrix P
    // Only has n non-zero elements, all of which are '1'
    // Can be stored in O(n) space
    std::vector<size_t>* P = new std::vector<size_t>(n, 0);
    for (size_t i = 0; i < n; ++i) {
        P->at(i) = i;
    }


    Matrix* L = new Matrix {n, n};
    for (size_t i = 0; i < n; ++i) {
        (*L)(i, i) = 1;
    }

    Matrix* U = new Matrix {A};

    // for (size_t j = 0; j < n; ++j) {
    //     for (size_t i = j + 1; i < n; ++i) {
    //         (*L)(i, j) = (*U)(i, j) / (*U)(j, j);
    //         for (size_t k = 0; k < n; ++k) {
    //             (*U)(i, k) -= (*L)(i, j) * (*U)(j, k); 
    //         }
    //     }
    // }

    for (size_t j = 0; j < n; ++j) {
        // Pivoting
        double pivot_val = 0;
        size_t row_pivot = 0;
        for (size_t i = j; i < n; ++i) {
            if (pivot_val < std::abs((*U)(i, j))) {
                pivot_val = std::abs((*U)(i, j));
                row_pivot = i;
            }
        }
        // pivot_val == 0 => singular matrix
        std::swap((*P)[j], (*P)[row_pivot]);
        // swap U->(j, :), U->(row_pivot, :)
        // https://www.clear.rice.edu/comp422/assignments/assignment2.html
        for (size_t k = 0; k < n; ++k) {
            std::swap((*U)(j, k), (*U)(row_pivot, k));
        }
        // swap L->(j, :j), L->(row_pivot, :j)
        for (size_t k = 0; k < j; ++k) {
            std::swap((*L)(j, k), (*L)(row_pivot, k));
        }
        // потенциально здесь тоже прагму
        for (size_t i = j + 1; i < n; ++i) {
            (*L)(i, j) = (*U)(i, j) / (*U)(j, j);
            #pragma omp parallel for
            for (size_t k = 0; k < n; ++k) {
                (*U)(i, k) -= (*L)(i, j) * (*U)(j, k);
            }
        }
    }


    return std::make_tuple(P, L, U);
}