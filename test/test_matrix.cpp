#include "../utility/catch.hpp"
#include "../utility/matrix.h"
#include <iostream>

using namespace OFEC;

TEST_CASE("eigen decomposition", "[eigendecomp]") {
	std::vector<std::vector<real>> data = { {-5,-5,0},{-3,-3,0},{-1,-1,0},{1,1,0},{3,3,0},{5,5,0},{-1,1,0},{1,-1,0},{0,0,1},{0,0,-1} };
	matrix mat(data.size(), 3);
	for (size_t i = 0; i < data.size(); ++i) {
		mat[i] = data[i];
	}
	matrix cov_mat;
	Vector mean;
	mat.covariance_matrix(cov_mat, mean);
	std::vector<real> eigen_value(cov_mat.row());
	std::vector<Vector> eigen_vector(cov_mat.row(), Vector(cov_mat.row()));
	cov_mat.eigendecomposition(eigen_value, eigen_vector);
	for (size_t i = 0; i < cov_mat.row(); ++i) {
		for (size_t j = 0; j < 3; ++j) {
			std::cout << eigen_vector[i][j] * sqrt(eigen_value[i]) << " ";
		}
		std::cout << std::endl;
	}
}

