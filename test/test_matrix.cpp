#define BOOST_TEST_MODULE matrix
#include <boost/test/unit_test.hpp>

#include "../utility/matrix.h"

using namespace OFEC;

BOOST_AUTO_TEST_SUITE(matrix_test)


BOOST_AUTO_TEST_CASE(test_case1)
{

	matrix a(3);
	normal rand(0.2);
	
	a.generate_rotation(&rand, 1e6);
	a.identify();
	a.transpose();
	a.inverse();
	a.diagonalize(&rand, 1e6);
	std::cout << a.compute_determinant(a.data(), a.data().size()) << std::endl;
	std::cout << a.is_inverse() << std::endl;
	std::cout << a.is_orthogonal() << std::endl;
	std::cout << a.is_identity() << std::endl;
	for (int i = 0; i < 3;i++) {
		for (int j = 0; j < 3;j++) {
			std::cout << a[i][j] << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
BOOST_AUTO_TEST_CASE(test_case2)
{

	matrix a(3);
	normal rand(0.2);
	
	a.generate_rotation(&rand, 1e6);
	a.orthonormalize();
	a.diagonalize(&rand, 1e6);
	std::cout << a.compute_determinant(a.data(), a.data().size()) << std::endl;
	std::cout << a.is_inverse() << std::endl;
	std::cout << a.is_orthogonal() << std::endl;
	std::cout << a.is_identity() << std::endl;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			std::cout << a[i][j] << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
BOOST_AUTO_TEST_CASE(test_case3)
{

	matrix a(3);
	
	double vec[3] = { 10,9,8 };
	a.set_col(vec, 3, 3);
	//a.set_row(vec, 2, 2);
	a.transpose();
	a.inverse();

	std::cout << a.compute_determinant(a.data(), a.data().size()) << std::endl;
	std::cout << a.is_inverse() << std::endl;
	std::cout << a.is_orthogonal() << std::endl;
	std::cout << a.is_identity() << std::endl;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			std::cout << a[i][j] << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}



BOOST_AUTO_TEST_SUITE_END()

