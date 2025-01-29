/********* Begin Register Information **********
{
	"description": "find the dimension with mininum intersection",
	"dependency on libraries": [ "Armadillo", "mlpack" ]
}
*********** End Register Information **********/

#include "../catch_amalgamated.hpp"
#include <mlpack/core.hpp>
#include <mlpack/methods/kde/kde.hpp>
#include <algorithm>

TEST_CASE("find the dimension with mininum intersection", "[statistics][data process]") {
	using namespace mlpack;
	std::vector<std::vector<double>> data1 = {
		{1,1},{1,2},{2,1},{2,2}
	};
	std::vector<std::vector<double>> data2 = {
		{3,1},{3,2},{4,1},{4,2}
	};
	arma::mat class1(2, data1.size()), class2(2, data2.size());
	for (size_t i = 0; i < data1.size(); ++i) {
		class1.col(i) = arma::vec(data1[i].data(), 2, false);
	}
	for (size_t i = 0; i < data2.size(); ++i) {
		class2.col(i) = arma::vec(data2[i].data(), 2, false);
	}
	std::vector<double> intersections(2), pivot(2);
	for (size_t j = 0; j < 2; ++j) {
		double stddev = arma::stddev(arma::join_rows(class1.row(j), class2.row(j)));
		double bandwith = 1.06 * stddev * pow(data1.size() + data2.size(), -0.2);
		KDE<GaussianKernel, EuclideanDistance, arma::mat> kde1(0.05, 0.0, GaussianKernel(bandwith)), kde2(0.05, 0.0, GaussianKernel(bandwith));
		kde1.Train(class1.row(j));
		kde2.Train(class2.row(j));
		double min_val = std::min(class1.row(j).min(), class2.row(j).min());
		double max_val = std::max(class1.row(j).max(), class2.row(j).max());
		arma::mat query = arma::linspace(min_val, max_val, 10).t();
		//std::cout << "query:" << std::endl;
		//std::cout << query << std::endl;
		arma::vec estimations1, estimations2;
		kde1.Evaluate(query, estimations1);
		kde2.Evaluate(query, estimations2);
		//std::cout << "estimations1:" << std::endl;
		//std::cout << estimations1.t() << std::endl;
		//std::cout << "estimations2:" << std::endl;
		//std::cout << estimations2.t() << std::endl;
		arma::vec pdf1 = estimations1 / arma::accu(estimations1);
		arma::vec pdf2 = estimations2 / arma::accu(estimations2);
		//std::cout << "pdf1:" << std::endl;
		//std::cout << pdf1.t() << std::endl;
		//std::cout << "pdf2:" << std::endl;
		//std::cout << pdf2.t() << std::endl;
		intersections[j] = arma::accu(arma::min(pdf1, pdf2));
		arma::vec diff = pdf1 - pdf2;
		int index = -1;
		for (size_t i = 0; i < diff.n_elem - 1; ++i) {
			if (diff(i) * diff(i + 1) <= 0) {
				index = i;
				break;
			}
		}
		pivot[j] = (query(0, index) + query(0, index + 1)) / 2;
	}
	size_t min_dim = 0;
	double min_intersection = intersections[0];
	for (size_t j = 1; j < 2; ++j) {
		if (intersections[j] < min_intersection) {
			min_intersection = intersections[j];
			min_dim = j;
		}
	}
	std::cout << min_dim << std::endl;
	std::cout << pivot[min_dim] << std::endl;
}
