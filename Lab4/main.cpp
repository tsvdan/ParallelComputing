#include <vector>
#include <iostream>
#include <cmath>
#include <functional>
#include <omp.h>

using namespace std;

const double EPSILON{ 1e-7 };
const int N{ 1'000'000 };

vector<double> operator+(vector<double> lhs, const vector<double>& rhs) {
    for (int i = 0; i < lhs.size(); ++i) {
        lhs[i] += rhs[i];
    }
    return lhs;
}

ostream& operator<<(ostream& os, const vector<double>& rhs) {
    for (int i = 0; i < rhs.size(); ++i) {
        os << rhs[i] << ' ';
    }
    return os;
}

using fn_type = function<double(double, const vector<double>&)>;

int main() {
// INIT
    vector<double> U_0{ 2, 1 };
    double t{ 2 };
    vector<fn_type> F{
        []   (double t, const vector<double>& U) -> double { return 3 * U[0] - U[0] * U[0] - U[0] * U[1]; }
        ,[]  (double t, const vector<double>& U) -> double { return U[1] - U[1] * U[1] + U[0] * U[1]; }
    };
    // dU(t)/dt = F(t, U(t));


/* вот из такого всё начиналось (в одну строчку :) */
// for (int i = 0; i < 50; ++i)
//     U_n = U_n + [&, a=0.0, b=t, functions=F] (vector<double>& U) -> vector<double> {
//         vector<double> result(U);
//         double step = (b - a) / N;
//         for (int j = 0; j < functions.size(); ++j) {
//             result[j] = step * [=] () -> double {
//                 double sum;
//                 #pragma omp parallel for num_threads(12) reduction(+: sum)
//                 for (int i = 1; i < N; ++i) {
//                     sum += functions[j](a + 0.5 * (step * (i-1) + step * i), U);
//                 }
//                 return sum;
//             }();
//         }
//         return result;
//     }(U_n);


    double step = (t - 0.0) / N;
    vector<vector<double>> I_k(U_0.size());
    for (int j = 0; j < I_k.size(); ++j) {
        I_k[j] = vector<double>(N - 1);
    }
    vector<vector<double>> U_k(N);
    for (int i = 0; i < N; ++i) {
        U_k[i].resize(U_0.size());
        for (int j = 0; j < U_0.size(); ++j) {
            U_k[i][j] = U_0[j];
        }
    }

    
    for (int _ = 0; _ < 200; ++_)  // заменить на while
    {
        #pragma omp parallel for num_threads(12)
        for (int i = 0; i < N - 1; ++i) {
            for (int j = 0; j < I_k.size(); ++j) {  // Интегрирование по правилу трапеций 
                double l = step / 2; 
                I_k[j][i] = l * (F[j]((0.0 + i * step), U_k[i]) + F[j](0.0 + (i + 1) * step, U_k[i]));
            }
        }

        for (int j = 0; j < I_k.size(); ++j) {
            for (int i = 1; i < N; ++i) {
                U_k[i][j] = U_k[i-1][j] + I_k[j][i-1];
            }
        }
        cout << U_k[N-1] << endl;
    }

    cout << "Решение U(t) = ( " << U_k[N-1] << ")" << endl;
    cout << "При t = " << t << endl;
}
