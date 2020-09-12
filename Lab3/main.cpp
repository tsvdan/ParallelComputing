#include "../Lab2/Matrix.h"
#include <iostream>
#include <omp.h>
#include <random>
#include <tuple>

using std::cout;
using std::endl;

std::tuple<Matrix*, Matrix*, Matrix*> random_system(size_t n) {
    Matrix* x = new Matrix{ n, 1, 0.0 };
    Matrix* U = new Matrix{ n, n, 0.0 };  // upper triangular

    std::random_device rd;
    std::default_random_engine gen{ rd() };
    std::uniform_real_distribution<double> U_upper_dist{ -0.1, 0.1 };  // подумать
    std::uniform_int_distribution<int>  x_dist{ 1, 9 };
    std::uniform_int_distribution<int>  diag_dist{ 1, 9 };
    std::uniform_real_distribution<double> coin{ 0, 1 };
    for (int i = 0; i < n; ++i) {
        (*x)(i, 0) = x_dist(gen) * (coin(gen) > 0.5 ? 1 : -1);
        (*U)(i, i) = diag_dist(gen) * (coin(gen) > 0.5 ? 1 : -1);
        for (int j = i + 1; j < n; ++j) {
            (*U)(i, j) = U_upper_dist(gen);
        }
    }

    for (int i = 1; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            (*U)(i, j) += (coin(gen) > 0.5 ? 1 : -1) * coin(gen) * (*U)(0, j);
        }
    }

    cout << *U << endl;
    cout << *x << endl;
    Matrix* b = new Matrix{ *U * *x };

    return std::make_tuple(U, x, b);
}


int main() {
    auto Axb = random_system(5);
    Matrix* A = std::get<0>(Axb);
    Matrix* X = std::get<1>(Axb);
    Matrix* B = std::get<2>(Axb);

    // Matrix A{2, 2, {1, 2, 1, 1}}; // -- минимальный пример для которого метод Якоби (и Зейделя*) не сх 
    // Matrix B{2, 1, {4, 3}};
    // cout << A * Matrix{2, 1, {2, 1}} - B << endl;
    
    auto time_start_serial = omp_get_wtime();
    omp_set_num_threads(1);

    auto jacobi_m = [] (const Matrix& a, const Matrix& b) -> Matrix {
        // a = L + D + U
        // ax = Dx + (L+U)x = b
        // x^{k+1} = D^{-1}b - D^{-1}(L+U)x^k  -- метод Якоби
        Matrix x(a.columns(), 1, 0.0);
        for (int k = 0; k < 500; ++k) {
            Matrix x_next(x.rows(), x.columns());
            for (int i = 0; i < a.rows(); ++i) {
                double dot = 0;  // (L+U)_i . x
                #pragma omp parallel for reduction(+: dot)
                for (int j = 0; j < a.columns(); ++j) {
                    if (j != i) dot += a(i, j) * x(j, 0);
                }
                x_next(i, 0) = (b(i, 0) - dot) / a(i, i);
            }
            x = x_next;
        }
        return x;
    };

    cout << jacobi_m(*A, *B) << endl;

    auto zeidel_relax_m = [] (const Matrix& a, const Matrix& b, double omega) -> Matrix {
        Matrix x(a.columns(), 1, 0.0);
        for (int k = 500; k > 0; --k) {
            Matrix x_next(x);
            x_next *= (1 - omega);
            for (int i = 0; i < a.rows(); ++i) {
                double dot_lower = 0;
                double dot_upper = 0;
                // до диагонали
                #pragma omp parallel for reduction(+: dot_lower)
                for (int j = 0; j < i; ++j) {
                    dot_lower += a(i, j) * x_next(j, 0);
                }

                // после
                #pragma omp parallel for reduction(+: dot_upper)
                for (int j = i + 1; j < a.columns(); ++j) {
                    dot_upper += a(i, j) * x(j, 0);
                }
                x_next(i, 0) += (b(i, 0) - dot_lower - dot_upper) * omega / a(i, i);
            }
            x = x_next;
        }
        return x;
    };

    cout << zeidel_relax_m(*A, *B, 0.56) << endl;


    auto time_end_serial   = omp_get_wtime();
    cout << "Последовательное время выполения: " << time_end_serial - time_start_serial << "\n\n" << endl;





    omp_set_num_threads(4);
    auto time_start_parallel = omp_get_wtime();
    cout << jacobi_m(*A, *B) << endl;
    cout << zeidel_relax_m(*A, *B, 0.56) << endl;
    auto time_end_parallel   = omp_get_wtime();
    cout << "Параллельное время выполения: " << time_end_parallel - time_start_parallel << endl;
}