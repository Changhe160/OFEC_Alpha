#include "../../utility/catch.hpp"
#include "../../utility/nondominated_sorting/fast_sort.h"
#include "../../utility/nondominated_sorting/deductive_sort.h"
#include "../../utility/nondominated_sorting/filter_sort.h"
#include <iostream>
#include "../../utility/functional.h"
#include "../../utility/environment_selection/selection_methods.h"

using namespace ofec;

TEST_CASE("fast nondominated sorting", "[FNS]") {
	std::vector<std::vector<int>> int_2d_array{ {2,4},{4,8},{6,12},{3,3},{6,6},{9,9},{4,2},{8,4},{12,6} };
	std::vector<std::vector<int>*> data;
	for (auto& i : int_2d_array)
		data.emplace_back(&i);
	std::vector<int> rank;
	std::vector<OptimizeMode> opt_mode(2, OptimizeMode::kMinimize);

	nd_sort::fastSort<int>(data, rank, opt_mode);
    for (auto i : rank)
        std::cout << i << " ";
    std::cout << std::endl;

	nd_sort::fastSortP<int>(data, rank, opt_mode, 8);
	for (auto i : rank)
		std::cout << i << " ";
	std::cout << std::endl;
}

TEST_CASE("Deductive Sort", "[DeductiveSort]") {
	std::vector<std::vector<int>> int_2d_array{ {2,4},{4,8},{6,12},{3,3},{6,6},{9,9},{4,2},{8,4},{12,6} };
	std::vector<std::vector<int>*> data;
	for (auto& i : int_2d_array)
		data.emplace_back(&i);
	std::vector<int> rank;
	std::vector<OptimizeMode> opt_mode(2, OptimizeMode::kMinimize);

	NS::deductiveSort<int>(data, rank, opt_mode);
	for (auto i : rank)
		std::cout << i << " ";
	std::cout << std::endl;
}

TEST_CASE("Filter Sort", "[FilterSort]") {
//    std::vector<std::vector<float>> int_2d_array{ {2,4},{4,8},{6,12},{3,3},{6,6},{9,9},{4,2},{8,4},{12,6} };
    const size_t N = 250, M = 2;

    std::vector<std::vector<float>> int_2d_array(N, std::vector<float>(M));
    for (int i = 0; i < N; ++i) {
        int_2d_array[i][0] = rand();
        int_2d_array[i][1] = 1;
    }
    std::vector<std::vector<float>*> data;
    for (auto& i : int_2d_array)
        data.emplace_back(&i);
    std::vector<int> rank;
    std::vector<OptimizeMode> opt_mode({ OptimizeMode::kMinimize, OptimizeMode::kMaximize });

    const int N_ = data.size(), M_ = data[0]->size();
    std::cout << N_ << " " << M_ << std::endl;

    nd_sort::fastSort<float>(data, rank, opt_mode);
    for (auto i : rank)
        std::cout << i << " ";
    std::cout << std::endl;

    nd_sort::fastSortP<float>(data, rank, opt_mode, 8);
    for (auto i : rank)
        std::cout << i << " ";
    std::cout << std::endl;

    nd_sort::filterSort<float>(data, rank, opt_mode);
    for (auto i : rank)
        std::cout << i << " ";
    std::cout << std::endl;

    nd_sort::filterSortP<float>(data, rank, opt_mode);
    for (auto i : rank)
        std::cout << i << " ";
    std::cout << std::endl;
}

TEST_CASE("template nondominated sorting", "[TNS]") {
    std::vector<std::vector<int>> int_2d_array{ {2,4},{4,8},{6,12},{3,3},{6,6},{9,9},{4,2},{8,4},{12,6} };
    std::vector<std::vector<int>*> data;
    for (auto& i : int_2d_array)
        data.emplace_back(&i);
    std::vector<int> rank;
    std::vector<OptimizeMode> opt_mode(2, OptimizeMode::kMinimize);

    nd_sort::fastSort<int>(data, rank, opt_mode);
    for (auto i : rank)
        std::cout << i << " ";
    std::cout << std::endl;
    std::cout << std::endl;
    auto r = vectorNondominatedSorting(int_2d_array,opt_mode);
    for (auto i : r)
        std::cout << i << " ";
    std::cout << std::endl;
}