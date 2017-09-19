#define BOOST_TEST_MODULE matrix
#include <boost/test/unit_test.hpp>

#include "../utility/matrix.h"

using namespace OFEC;

BOOST_AUTO_TEST_SUITE(matrix_test)


BOOST_AUTO_TEST_CASE(test_case1)
{
	std::cout << "********This is test_case1 :********" << std::endl;
	matrix a(3);
	int size = a.data().size();
	normal rand(0.2);
	double temp[3][3] = { {3,2,4},{2,0,2},{4,2,3} };
	std::vector<Vector> data(3, Vector(3));
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			data[i][j] = temp[i][j];
		}
	}
	a.set(data);
	

	//a.generate_rotation(&rand, 1e6);
	//a.identify();
	//a.transpose();
	//a.inverse();
	//a.diagonalize(&rand, 1e6);
	//a.eigendecomposition();
	//a.orthonormalize();
	a.classical_orthonormalize();
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
	for (int i = 0; i < a.data().size();i++) {
		for (int j = 0; j < a.data().size();j++) {
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
	normal rand(0.5);
	
	//a.generate_rotation(&rand, 1e6, orthonormalize_mode::modified);
	a.generate_rotation(&rand, 1e6);
	//a.classical_orthonormalize();
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
	matrix a(2),b(2);
	
	double vec[2] = { 10,9 };
	a.set_col(vec, 2, 2);
	b.set_row(vec, 2, 2);
	//a.set_row(vec, 2, 2);
	//a.transpose();
	//a.inverse();
	//a.diagonalize(2);
	//a.inverse();
	//a.classical_orthonormalize();
	//a.orthonormalize();
	//a=b*a;
	a*=b;
	//std::cout << (a==b) << std::endl;
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

