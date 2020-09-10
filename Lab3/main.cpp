#include "../Lab2/Matrix.h"
#include <iostream>
#include <omp.h>

using std::cout;
using std::endl;

int main() {
    Matrix A(2, 2, 0.0);
    A(0,0) = 1;
    A(0,1) = 3;
    A(1,0) = -2;
    A(1,1) = 7;
    
    cout << A << endl;

    Matrix B(2, 1, 1.0);
    B(0,0) = 4;
    B(1,0) = 9;
    
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
                for (int j = 0; j < a.columns(); ++j) {
                    if (j != i) dot += a(i, j) * x(j, 0);
                }
                x_next(i, 0) = (b(i, 0) - dot) / a(i, i);
            }
            x = x_next;
        }
        return x;
    };

    cout << jacobi_m(A, B) << endl;

    auto zeidel_relax_m = [] (const Matrix& a, const Matrix& b, double omega) -> Matrix {
        Matrix x(a.columns(), 1, 0.0);
        for (int k = 500; k > 0; --k) {
            Matrix x_next(x);
            x_next *= (1 - omega);
            for (int i = 0; i < a.rows(); ++i) {
                double dot = 0;
                // до диагонали
                for (int j = 0; j < i; ++j) {
                    dot += a(i, j) * x_next(j, 0);
                }

                // после
                for (int j = i + 1; j < a.columns(); ++j) {
                    dot += a(i, j) * x(j, 0);
                }
                x_next(i, 0) += (b(i, 0) - dot) * omega / a(i, i);
            }
            x = x_next;
        }
        return x;
    };

    cout << zeidel_relax_m(A, B, 0.45) << endl;





    auto time_end_serial   = omp_get_wtime();
    cout << "Последовательное время выполения: " << time_end_serial - time_start_serial << endl;






    auto time_start_parallel = omp_get_wtime();
    auto time_end_parallel   = omp_get_wtime();
    cout << "Параллельное время выполения: " << time_end_parallel - time_start_parallel << endl;
}