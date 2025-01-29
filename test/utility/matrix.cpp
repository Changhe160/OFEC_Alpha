#include "../../utility/catch.hpp"
#include "../../utility/linear_algebra/matrix.h"
#include <iostream>

using namespace ofec;

TEST_CASE("Matrix eigen decomposition", "[matrix]") {
	Matrix A(4);
	A[0] = { 1.0000, 0.5000, 0.3333, 0.2500 };
	A[1] = { 0.5000, 1.0000, 0.6667, 0.5000 };
	A[2] = { 0.3333, 0.6667, 1.0000, 0.7500 };
	A[3] = { 0.2500, 0.5000, 0.7500, 1.0000 };
	std::cout << A << std::endl;
	Matrix V;
	Vector d;
	A.eigenDecompose(V, d);
	std::cout << V << std::endl;
	std::cout << d << std::endl;
	std::cout << std::endl;;

	std::cout << A * V - V * Matrix::diag(d) << std::endl;
}

TEST_CASE("Matrix calculation", "[matrix]") {
	Matrix A(4);
	A[0] = { 1.0000, 0.5000, 0.3333, 0.2500 };
	A[1] = { 0.5000, 1.0000, 0.6667, 0.5000 };
	A[2] = { 0.3333, 0.6667, 1.0000, 0.7500 };
	A[3] = { 0.2500, 0.5000, 0.7500, 1.0000 };
	std::cout << A << std::endl;
	std::cout << A + A << std::endl;
}

TEST_CASE("Vector percentile", "[vector]") {
	Vector vec({ 0.5377, 1.8339, -2.2588, 0.8622, 0.3188, -1.3077, -0.4336 });
	std::cout << " 42%:\t" << prctile(vec, 42) << std::endl;
	std::cout << "  0%:\t" << prctile(vec, 0) << std::endl;
	std::cout << " 25%:\t" << prctile(vec, 25) << std::endl;
	std::cout << " 50%:\t" << prctile(vec, 50) << std::endl;
	std::cout << " 75%:\t" << prctile(vec, 75) << std::endl;
	std::cout << "100%:\t" << prctile(vec, 100) << std::endl;
}

TEST_CASE("Vector sorting", "[vector]") {
	Vector vec({ 9, 0, -7, 5, 3, 8, -10, 4, 2 });
	std::cout << "Vector:\t\t" << vec << std::endl;
	std::cout << "Sorted vector:\t" << vec.slice(sort(vec)) << std::endl;
}
