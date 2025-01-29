/********* Begin Register Information **********
{
	"dependency on libraries": [ "Eigen" ]
}
*********** End Register Information **********/

#include "../catch_amalgamated.hpp"
#include "../../utility/linear_algebra/eigen.h"
#include <iostream>

TEST_CASE("computational cost evaluation") {
	using namespace Eigen;
	size_t big_size = 5000;
	ofec::Uniform unif(0.5);
	auto big_mat_1 = ofec::randMatXd(big_size, big_size, &unif);
	auto mutli_mat = big_mat_1 * big_mat_1;
	//auto abs_mat = (big_mat_1 - big_mat_2).cwiseAbs();
	//double min_abs_value = abs_mat.minCoeff();
	double min_abs_value = (mutli_mat - big_mat_1).cwiseAbs().minCoeff();
	std::cout << min_abs_value << std::endl;
}

TEST_CASE("Matrix operations in Eigen", "[Eigen][matrix]") {
	using namespace Eigen;

	MatrixXd A(2, 2), B(2, 3);
	A <<
		1, 2,
		3, 4;
	std::cout << "A=\n" << A << std::endl;
	B(0, all) << A(0, all), 5;
	B(1, all) << A(1, all), 6;
	B(0, all) << A(1, all), 5;
	std::cout << "B=\n" << B << std::endl;

	MatrixXd C(4, 4);
	C <<
		1.0000, 0.5000, 0.3333, 0.2500,
		0.5000, 1.0000, 0.6667, 0.5000,
		0.3333, 0.6667, 1.0000, 0.7500,
		0.2500, 0.5000, 0.7500, 1.0000;
	SelfAdjointEigenSolver<MatrixXd> es(C);
	std::cout << "C=\n" << C << std::endl;
	std::cout << "eigval=\n" << es.eigenvalues() << std::endl;
	std::cout << "eigvec=\n" << es.eigenvectors() << std::endl;

	MatrixXd D(2, 2);
	D <<
		5, 6,
		7, 8;
	std::cout << "D=\n" << D << std::endl;
	MatrixXd tmp1(2, 4);
	tmp1 << A, D;
	std::cout << "horzcat(A, D)=\n" << tmp1 << std::endl;
	MatrixXd tmp2(4, 2);
	tmp2 <<
		A,
		D;
	std::cout << "vertcat(A, D)=\n" << tmp2 << std::endl;
}

void testFunEigen(const Eigen::Ref<const Eigen::MatrixXd> &x) {
	using namespace Eigen;
	for (auto i = 0; i < x.rows(); ++i) {
		std::cout << x.row(i) << std::endl;
	}
}

TEST_CASE("Vector operations in Eigen", "[Eigen][vector]") {
	using namespace Eigen;

	VectorXd a = VectorXd::LinSpaced(5, -2, 2);
	std::cout << "a=\n" << a << std::endl;
	a = a.cwiseMax(0);
	std::cout << "a=\n" << a << std::endl;
	VectorXd b = VectorXd::LinSpaced(5, 2, -2);
	std::cout << "b=\n" << b << std::endl;

	MatrixXd E = VectorXd::LinSpaced(10, 0, 9).reshaped(5, 2);
	std::cout << "E=" << std::endl;
	testFunEigen(E);
	VectorXi ind(3);
	ind << 0, 2, 4;
	std::cout << "E(ind)=" << std::endl;
	testFunEigen(E(ind, all));

	VectorXd c = VectorXd::LinSpaced(10, 5, -4);
	std::cout << "c = " << c.transpose() << std::endl;
	std::cout << "median(c) = " << ofec::median(c) << std::endl;
	std::cout << "median(c.head(3)) = " << ofec::median(c.head(3)) << std::endl;
	std::cout << "c(sort(c)) = " << c(ofec::sort(c)).transpose() << std::endl;
	std::cout << "find(c.array() > 0) = " << ofec::find(c.array() > 0).transpose() << std::endl;
}

TEST_CASE("test cholesky decompose","[Eigen][cholesky decompose]") {
	size_t p = 5;
	size_t n = 5;
	Eigen::MatrixXd A;
	Eigen::MatrixXd b;
	Eigen::MatrixXd X;
	A = Eigen::MatrixXd::Random(p,n);
	b = Eigen::MatrixXd::Ones(p, 1);
	X = A.llt().solve(b);
}

TEST_CASE("test QR decompose", "[Eigen][QR decompose]") {
	size_t p = 5;
	size_t n = 3;
	Eigen::MatrixXd A;
	Eigen::MatrixXd b;
	Eigen::MatrixXd X;
	A = Eigen::MatrixXd::Random(p, n);
	for (size_t i = 0; i < p; ++i) {
		for (size_t j = 0; j < n; ++j) {
			A(i,j) = 1. + i + j;
			std::cout << A(i, j) << std::endl;
		}
	}
	b = Eigen::MatrixXd::Ones(p, 1);
	X = A.colPivHouseholderQr().solve(b);
	//output x
	for (size_t i = 0; i < n; ++i) {
		std::cout << X(i) << std::endl;
	}
	//系数代入反求解
	std::vector<double> re;
	for (size_t i = 0; i < p; ++i) {
		auto temp = A.row(i) * X;
		re.push_back(temp.value());
		std::cout << temp.value() << std::endl;
	}
}
