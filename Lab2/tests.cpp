/* --- WRITE BETTER TESTS --- */
#include "Matrix.h"
#include <iostream>
#include <random>
#include <tuple>
#include "../catch.hpp"

using std::cout;
using std::endl;

bool test_constructors() {
    // write a better test
    Matrix zero_matrix(3, 4);
    Matrix constant_matrix(3, 4, 2);
    Matrix copied_constant_matrix(constant_matrix);

    // mutate constant_m and copied_constant_m
    // to show they have separate containers for values
    constant_matrix(2, 2) = 42;
    copied_constant_matrix(2, 2) = -39;
    Matrix assigned_matrix;
    assigned_matrix = copied_constant_matrix;
    assigned_matrix(1, 1) = 5;

    // cout << zero_matrix << "\n\n";
    // cout << constant_matrix << "\n\n";
    // cout << copied_constant_matrix << "\n\n";
    // cout << assigned_matrix << "\n\n";
    // cout << copied_constant_matrix << "\n\n";
    return true;
}

bool test_assign_ops() {
    // what was i doing here
    Matrix A {3, 3, 
    {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    }};
    Matrix I {3, 3, 
    {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    }};
    // cout << A << "\n\n";

    // cout << A  * 2 << "\n\n";
    // cout << -2 * I << "\n\n";
    // cout << A - 5 * I << "\n\n";
    // cout << 5 * A - I << "\n\n";
    
    return true;
}

TEST_CASE( "Random multipication 3x2 x 2x3 and 2x3 x 3x2" ) {
    std::random_device rd;
    std::minstd_rand gen {rd()};
    std::uniform_real_distribution<double> dis {-2, 2};

    std::vector<double> A_v {
        dis(gen), dis(gen),
        dis(gen), dis(gen),
        dis(gen), dis(gen)
    };
    std::vector<double> B_v {
        dis(gen), dis(gen), dis(gen),
        dis(gen), dis(gen), dis(gen)
    };
    Matrix A {3, 2, A_v};
    Matrix B {2, 3, B_v};

    Matrix C {3, 3, {
        A_v[0] * B_v[0] + A_v[1] * B_v[3], A_v[0] * B_v[1] + A_v[1] * B_v[4], A_v[0] * B_v[2] + A_v[1] * B_v[5],
        A_v[2] * B_v[0] + A_v[3] * B_v[3], A_v[2] * B_v[1] + A_v[3] * B_v[4], A_v[2] * B_v[2] + A_v[3] * B_v[5],
        A_v[4] * B_v[0] + A_v[5] * B_v[3], A_v[4] * B_v[1] + A_v[5] * B_v[4], A_v[4] * B_v[2] + A_v[5] * B_v[5]
    }};
    Matrix D {2, 2, {
        B_v[0] * A_v[0] + B_v[1] * A_v[2] + B_v[2] * A_v[4], B_v[0] * A_v[1] + B_v[1] * A_v[3] + B_v[2] * A_v[5], 
        B_v[3] * A_v[0] + B_v[4] * A_v[2] + B_v[5] * A_v[4], B_v[3] * A_v[1] + B_v[4] * A_v[3] + B_v[5] * A_v[5]
    }};
    REQUIRE( (all_close(A*B, C) && all_close(B*A, D)) );
}

TEST_CASE( "LU-decomposition" ) {
    std::random_device rd;
    std::minstd_rand gen {rd()};
    std::uniform_real_distribution<double> dis {-2, 2};

    std::vector<double> A_v {
        dis(gen), dis(gen), dis(gen),
        dis(gen), dis(gen), dis(gen),
        dis(gen), dis(gen), dis(gen)
    };
    Matrix A {3, 3, A_v};
    auto plu = PLU_decompose(A);

    // cout << A << "\n\n";

    // P
    auto P = *std::get<0>(plu);
    Matrix P_ {3, 3};
    for (size_t i = 0; i < P.size(); ++i) {
        P_(i, P[i]) = 1;
    }

    // cout << P_ << "\n\n";

    auto L = *std::get<1>(plu);
    // cout << L << "\n\n";

    auto U = *std::get<2>(plu);
    // cout << U << "\n\n";

    REQUIRE( all_close(L * U, P_ * A) );
}

TEST_CASE() {
    auto should_be_five{ 0 };
    should_be_five += test_constructors();
    should_be_five += test_assign_ops();
    REQUIRE( should_be_five == 2 );
}