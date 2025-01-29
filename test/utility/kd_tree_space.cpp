/********* Begin Register Information **********
{
	"description": "k-d tree space"
}
*********** End Register Information **********/

#include "../catch_amalgamated.hpp"
#include "../../utility/kd-tree/kdtree_space.h"
#include "../../core/random/newran.h"

using namespace ofec;

TEST_CASE("k-d tree space", "[kdtree][space]") {
	size_t num_genes=10;
	size_t size_var = 2;
	std::vector<Real> ratios(num_genes, 1. / num_genes);
	nanoflann::KDTreeSpace<Real> a(ratios);
	std::vector<std::pair<Real, Real>> boundary(size_var);
	for (size_t i = 0; i < size_var; ++i) {
		boundary[i] = { -1,1 };
	}
	a.setInitBox(boundary);
	a.buildIndex();
	size_t mean_num_neighbors, min_num_neighbors, max_num_neighbors;
	std::list<size_t> neighbors;
	mean_num_neighbors = 0;
	min_num_neighbors = num_genes;
	max_num_neighbors = 0;
	for (size_t i = 0; i < num_genes; i++) {
		a.findNeighbor(i, neighbors);
		mean_num_neighbors += neighbors.size();
		if (max_num_neighbors < neighbors.size()) max_num_neighbors = neighbors.size();
		if (min_num_neighbors > neighbors.size()) min_num_neighbors = neighbors.size();
	}
	mean_num_neighbors /= num_genes;

	std::cout << num_genes << "\t" << min_num_neighbors << "\t" << mean_num_neighbors << "\t" << max_num_neighbors << std::endl;
	num_genes = 1000;
	a.inputRatioData(std::vector<Real>(num_genes, 1.0 / num_genes));
	a.buildIndex();
	mean_num_neighbors = 0;
	min_num_neighbors = num_genes;
	max_num_neighbors = 0;
	for (size_t i = 0; i < num_genes; i++) {
		a.findNeighbor(i, neighbors);
		mean_num_neighbors += neighbors.size();
		if (max_num_neighbors < neighbors.size()) max_num_neighbors = neighbors.size();
		if (min_num_neighbors > neighbors.size()) min_num_neighbors = neighbors.size();
	}
	mean_num_neighbors /= num_genes;

	std::cout << num_genes << "\t" << min_num_neighbors << "\t" << mean_num_neighbors << "\t" << max_num_neighbors << std::endl;
	num_genes = 10000;
	min_num_neighbors = num_genes;
	max_num_neighbors = 0;
	a.inputRatioData(std::vector<Real>(num_genes, 1.0 / num_genes));
	a.buildIndex();
	mean_num_neighbors = 0;
	for (size_t i = 0; i < num_genes; i++) {
		a.findNeighbor(i, neighbors);
		mean_num_neighbors += neighbors.size();
		if (max_num_neighbors < neighbors.size()) max_num_neighbors = neighbors.size();
		if (min_num_neighbors > neighbors.size()) min_num_neighbors = neighbors.size();
	}
	mean_num_neighbors /= num_genes;
	std::cout << num_genes << "\t" << min_num_neighbors << "\t" << mean_num_neighbors << "\t" << max_num_neighbors << std::endl;
}

TEST_CASE("k-d tree space merge and split", "[kdtree][merge][split]") {
	size_t num_genes = 5;
	size_t size_var = 2;
	Uniform rand(0.5);
	std::vector<Real> ratios(num_genes, 1. / num_genes);
	nanoflann::KDTreeSpace<Real> a(ratios);
	std::vector<std::pair<Real, Real>> boundary(size_var);
	for (size_t i = 0; i < size_var; ++i) {
		boundary[i] = { -5,5 };
	}
	a.setInitBox(boundary);
	a.buildIndex();
	a.regionShow();
	for (size_t i = 0; i < 2; i++) {
		auto idxs = a.regionsIndices();
		size_t idx_reg = *rand.nextElem(idxs.begin(), idxs.end());
		int dim = floor(size_var*rand.next());
		a.splitRegion(idx_reg,&dim);
		a.regionShow();
	}
	for (size_t i = 0; i < 5; i++) {
		auto idxs = a.regionsIndices();
		size_t idx_reg = *rand.nextElem(idxs.begin(), idxs.end());
		std::list<size_t> merged_regs;
		a.mergeRegion(idx_reg, merged_regs);
		a.regionShow();
	}
	for (size_t i = 0; i < 10; i++) {
		auto idxs = a.regionsIndices();
		size_t idx_reg = *rand.nextElem(idxs.begin(), idxs.end());
		a.splitRegion(idx_reg);
		a.regionShow();
	}
	for (size_t i = 0; i < 5; i++) {
		auto idxs = a.regionsIndices();
		size_t idx_reg = *rand.nextElem(idxs.begin(), idxs.end());
		std::list<size_t> merged_regs;
		a.mergeRegion(idx_reg, merged_regs);
		a.regionShow();
	}
}

TEST_CASE("k-d tree subtree", "[kdtree][subtree][split]") {
	size_t num_genes = 5;
	size_t size_var = 2;
	Uniform rand(0.5);
	std::vector<Real> ratios(num_genes, 1. / num_genes);
	nanoflann::KDTreeSpace<Real> a(ratios);
	std::vector<std::pair<Real, Real>> boundary(size_var);
	for (size_t i = 0; i < size_var; ++i) {
		boundary[i] = { -5,5 };
	}
	a.setInitBox(boundary);
	a.buildIndex();
	a.regionShow();
	std::cout << std::endl;

	/*size_t idx_region = 2;
	auto bbox = a.getBox(idx_region);
	size_t depth = a.getDepth(idx_region);
	a.ratioDivideTree(0, 4, bbox, depth+1);*/

	size_t idx = 1;
	size_t num_gene2 = 4;
	std::vector<Real> ratios2(num_gene2, 1. / num_gene2);
	nanoflann::KDTreeSpace<Real> b(ratios2);
	std::vector<std::pair<Real, Real>> boundary2(size_var);
	for (size_t i = 0; i < size_var; ++i) {
		boundary2[i] = a.getBox(idx)[i];
	}
	b.setInitBox(boundary2);
	b.buildIndex();
	b.regionShow();
	std::cout << std::endl;

	std::map<size_t, size_t> add_id;
	a.addSubtree(idx, b, add_id);
	a.regionShow();
	std::cout << std::endl;

	std::list<size_t> neighbors;
	a.findNeighbor(2, neighbors);
}
