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
    vector<double> U_n{ 1 };
    double t{ 3.14 / 2 };  // irrelevant
    vector<fn_type> F{
        []  (double t, const vector<double>& U) -> double { return cos(t); }
    };
    // dU(t)/dt = F(t, U(t));
// END INIT
    // for (int i = 0; i < 50; ++i) {
    //     cout << U_n << endl;
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
    // }

    //cout << U_n << endl;

// STILL ONE EQUATION
    double step = (t - 0.0) / N;
    vector<vector<double>> I_k(U_n.size());
    for (int j = 0; j < I_k.size(); ++j) {
        I_k[j] = vector<double>(N - 1);
    }
    vector<vector<double>> U_k(N);
    for (int i = 0; i < N; ++i) {
        U_k[i].resize(U_n.size());
        for (int j = 0; j < U_n.size(); ++j) {
            U_k[i][j] = U_n[j];
        }
    }

    
    for (int _ = 0; _ < 50; ++_)
    {
        #pragma omp parallel for num_threads(12)
        for (int i = 0; i < N - 1; ++i) {
            for (int j = 0; j < I_k.size(); ++j) {
                double l = step / 2;  //0.5 * ((0.0 + (i + 1) * step) - (0.0 + i * step)) = 0.5 * step * (i + 1 - i) = 0.5 * step * 1
                I_k[j][i] = l * (F[j]((0.0 + i * step), U_k[i]) + F[j](0.0 + (i + 1) * step, U_k[i]));
            }
        }

        for (int j = 0; j < I_k.size(); ++j) {
            for (int i = 1; i < N; ++i) {
                U_k[i][j] = U_k[i-1][j] + I_k[j][i-1];
            }
        }
    }

    cout << U_k[N-1] << endl;
}
