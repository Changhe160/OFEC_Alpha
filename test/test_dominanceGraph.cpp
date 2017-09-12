#define BOOST_TEST_MODULE dominanceGraph
#include <boost/test/unit_test.hpp>

#include "../utility/dominance_graph/dominance_graph.h"

BOOST_AUTO_TEST_SUITE(dominanceGraph_test)

using namespace dominance_graph;

BOOST_AUTO_TEST_CASE(test_case1) {
	
	vector < vector<double> > data = { { 2,3 },{ 5,0 },{ 4,4 },{ 3,2 },{ 1,1 },{ 3,6 },{ 0,3 },{ 4,1 },{ 4,7 },{ 5,6 },{ 6,4 },{ 6,7 },{ 6,2 },{ 1,2 },{ 0,5 },{ 1,4, },{ 0,7 },{ 1,6 } };
	graph test_graph(data);
	test_graph.quick_sort();
	test_graph.set_ranking();
	system("pause");

}

BOOST_AUTO_TEST_SUITE_END()