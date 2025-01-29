/********* Begin Register Information **********
{
	"description": "k-d tree search"
}
*********** End Register Information **********/

#include "../catch_amalgamated.hpp"
#include "../../utility/kd-tree/nanoflann.hpp"
#include "../../utility/kd-tree/data_adaptor.h"
#include "../../core/random/newran.h"

using namespace ofec;

TEST_CASE("k-d tree search", "[kdtree][search]") {
	Uniform rand(0.5);
	size_t num_point = 1000, size_var = 3;
	std::vector<std::vector<Real>> data(num_point,std::vector<Real>(size_var));
	for (size_t i = 0; i < num_point; i++) {
		for (size_t j = 0; j < size_var; j++) {
			data[i][j] = static_cast<Real>(rand.next());
		}
	}
	KDTreeVectorOfVectorsAdaptor<std::vector<std::vector<Real>>, Real> kd_tree(size_var, data, 2);

	const Real query_pt[3] = { 0.5, 0.5, 0.5 };
	size_t num_results = 5;
	std::vector<size_t> ret_index(num_results);
	std::vector<Real> out_dist_sqr(num_results);

	num_results = kd_tree.index->knnSearch(query_pt, num_results, ret_index.data(), out_dist_sqr.data());

	// In case of less points in the tree than requested:
	ret_index.resize(num_results);
	out_dist_sqr.resize(num_results);

	std::cout << "knnSearch(): num_results=" << num_results << "\n";
	for (size_t i = 0; i < num_results; i++)
		std::cout << "idx[" << i << "]=" << ret_index[i] << " dist[" << i << "]=" << out_dist_sqr[i] << std::endl;
	std::cout << "\n";
}
