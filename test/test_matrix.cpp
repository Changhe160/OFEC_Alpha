#define BOOST_TEST_MODULE matrix
#include <boost/test/unit_test.hpp>

#include "../utility/matrix.h"

using namespace OFEC;

BOOST_AUTO_TEST_SUITE(matrix_test)


BOOST_AUTO_TEST_CASE(test_case1)
{
	std::cout << "********This is test_case1 :********" << std::endl;
	matrix a(3);
	normal rand(0.2);
	
	a.generate_rotation(&rand, 1e6);
	//a.identify();
	//a.transpose();
	//a.inverse();
	//a.diagonalize(&rand, 1e6);
	a.eigendecomposition();
	std::cout << "eigenvalues are :" << std::endl;
	for (int i = 0; i < a.data().size(); i++) {
		std::cout << a.get_eigen_value()[i] << ' ';
	}
	std::cout << std::endl;
	
	std::cout << "eigenvectors are :" << std::endl;
	for (int i = 0; i < a.data().size(); i++) {
		for (int j = 0; j < a.data().size(); j++) {
			std::cout << a.get_eigen_vector()[i][j] << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	
	std::cout << a.determinant() << std::endl;
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
	std::cout << "********This is test_case2 :********" << std::endl;
	matrix a(3);
	normal rand(0.2);
	
	a.generate_rotation(&rand, 1e6);
	a.orthonormalize();
	//a.diagonalize(&rand, 1e6);
	std::cout << a.determinant() << std::endl;
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
	std::cout << "********This is test_case3 :********" << std::endl;
	matrix a(2);
	
	double vec[2] = { 10,9 };
	a.set_col(vec, 2, 2);
	//a.set_row(vec, 2, 2);
	//a.transpose();
	//a.inverse();
	a.diagonalize(2);
	a.inverse();

	std::cout << a.determinant() << std::endl;
	std::cout << a.is_inverse() << std::endl;
	std::cout << a.is_orthogonal() << std::endl;
	std::cout << a.is_identity() << std::endl;
	std::cout << a.is_diagonal() << std::endl;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			std::cout << a[i][j] << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
BOOST_AUTO_TEST_CASE(test_case4)
{
	std::cout << "********This is test_case4 :********" << std::endl;
	matrix a(2,3);     

	double vec[2] = { 10,9 };
	a.set_col(vec, 2, 3);
	//a.set_row(vec, 2, 3);
	a.transpose();
	//a.inverse();

	//std::cout << a.determinant() << std::endl;
	//std::cout << a.is_inverse() << std::endl;
	//std::cout << a.is_orthogonal() << std::endl;
	//std::cout << a.is_identity() << std::endl;
	for (int i = 0; i < a.data().size(); i++) {
		for (int j = 0; j < a.data()[i].size(); j++) {
			std::cout << a[i][j] << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}


BOOST_AUTO_TEST_SUITE_END()

