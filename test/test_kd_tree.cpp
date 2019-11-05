#include "../utility/catch.hpp"
#include "../utility/kd-tree/kdtree_space.h"
#include "../utility/kd-tree/nanoflann.hpp"
#include "../utility/random/newran.h"

using namespace OFEC;

TEST_CASE("k-d tree space", "[kdtreespace]") {
	size_t num_genes;
	size_t size_var = 10;
	//uniform rand(0.5);
	KDTreeSpace::PartitioningKDTree<real> a(size_var, 2);
	std::vector<std::pair<real, real>> boundary(size_var);
	for (size_t i = 0; i < size_var; ++i) {
		boundary[i] = { -5,5 };
	}
	a.setInitBox(boundary);
	size_t mean_num_neighbors, min_num_neighbors, max_num_neighbors;
	std::list<int> neighbors;
	num_genes = 100;
	a.inputData(std::vector<real>(num_genes, 1.0 / num_genes));
	a.buildIndex();
	mean_num_neighbors = 0;
	min_num_neighbors = num_genes;
	max_num_neighbors = 0;
	for (size_t i = 0; i < num_genes; i++) {
		a.find_neighbor(i, neighbors);
		mean_num_neighbors += neighbors.size();
		if (max_num_neighbors < neighbors.size()) max_num_neighbors = neighbors.size();
		if (min_num_neighbors > neighbors.size()) min_num_neighbors = neighbors.size();
	}
	mean_num_neighbors /= num_genes;

	std::cout << num_genes << "\t" << min_num_neighbors << "\t" << mean_num_neighbors << "\t" << max_num_neighbors << std::endl;
	num_genes = 1000;
	a.inputData(std::vector<real>(num_genes, 1.0 / num_genes));
	a.buildIndex();
	mean_num_neighbors = 0;
	min_num_neighbors = num_genes;
	max_num_neighbors = 0;
	for (size_t i = 0; i < num_genes; i++) {
		a.find_neighbor(i, neighbors);
		mean_num_neighbors += neighbors.size();
		if (max_num_neighbors < neighbors.size()) max_num_neighbors = neighbors.size();
		if (min_num_neighbors > neighbors.size()) min_num_neighbors = neighbors.size();
	}
	mean_num_neighbors /= num_genes;

	std::cout << num_genes << "\t" << min_num_neighbors << "\t" << mean_num_neighbors << "\t" << max_num_neighbors << std::endl;
	num_genes = 10000;
	min_num_neighbors = num_genes;
	max_num_neighbors = 0;
	a.inputData(std::vector<real>(num_genes, 1.0 / num_genes));
	a.buildIndex();
	mean_num_neighbors = 0;
	for (size_t i = 0; i < num_genes; i++) {
		a.find_neighbor(i, neighbors);
		mean_num_neighbors += neighbors.size();
		if (max_num_neighbors < neighbors.size()) max_num_neighbors = neighbors.size();
		if (min_num_neighbors > neighbors.size()) min_num_neighbors = neighbors.size();
	}
	mean_num_neighbors /= num_genes;
	std::cout << num_genes << "\t" << min_num_neighbors << "\t" << mean_num_neighbors << "\t" << max_num_neighbors << std::endl;
}

TEST_CASE("k-d tree search", "[kdtreesearch]") {
	uniform rand(0.5);
	size_t num_point = 1000, size_var = 3;
	std::vector<std::vector<real>> data(num_point,std::vector<real>(size_var));
	for (size_t i = 0; i < num_point; i++) {
		for (size_t j = 0; j < size_var; j++) {
			data[i][j] = static_cast<real>(rand.next());
		}
	}
	KDTreeVectorOfVectorsAdaptor<std::vector<std::vector<real>>, real> kd_tree(size_var, data, 2);


	const real query_pt[3] = { 0.5, 0.5, 0.5 };
	size_t num_results = 5;
	std::vector<size_t> ret_index(num_results);
	std::vector<real> out_dist_sqr(num_results);

	num_results = kd_tree.index->knnSearch(&query_pt[0], num_results, &ret_index[0], &out_dist_sqr[0]);

	// In case of less points in the tree than requested:
	ret_index.resize(num_results);
	out_dist_sqr.resize(num_results);

	std::cout << "knnSearch(): num_results=" << num_results << "\n";
	for (size_t i = 0; i < num_results; i++)
		std::cout << "idx[" << i << "]=" << ret_index[i] << " dist[" << i << "]=" << out_dist_sqr[i] << std::endl;
	std::cout << "\n";

}