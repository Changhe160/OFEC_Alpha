/*boost unit test for multi-objective sorting*/

#define BOOST_TEST_MODULE MO_sort
#include <boost/test/unit_test.hpp>
#include "../utility/MO_sort/dominance_quick_sort.h"
#include "../utility/MO_sort/fast_nondominated_sort.h"
#include "../utility/random/newran.h"

BOOST_AUTO_TEST_SUITE(MO_sort_test)

BOOST_AUTO_TEST_CASE(test_case1) {

	vector < vector<double> > data = { { 2,3 },{ 5,0 },{ 4,4 },{ 3,2 },{ 1,1 },{ 3,6 },{ 0,3 },{ 4,1 },{ 4,7 },{ 5,6 },{ 6,4 },{ 6,7 },{ 6,2 },{ 1,2 },{ 0,5 },{ 1,4, },{ 0,7 },{ 1,6 } };
	dominance_graph::graph dg(data);
	dg.quick_sort();
	cout << "quick sort compares " << dg.number() << " times" << endl;
	dg.set_ranking();
	auto & dg_result = dg.ranking();

}

BOOST_AUTO_TEST_CASE(test_case2) {
	vector < vector<double> > data = { { 2,3 },{ 5,0 },{ 4,4 },{ 3,2 },{ 1,1 },{ 3,6 },{ 0,3 },{ 4,1 },{ 4,7 },{ 5,6 },{ 6,4 },{ 6,7 },{ 6,2 },{ 1,2 },{ 0,5 },{ 1,4, },{ 0,7 },{ 1,6 } };
	vector<int> no = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
	vector<pair<int, vector<double>>> md;
	for (int i = 0; i < data.size(); ++i) {
		md.emplace_back(no[i], data[i]);
	}
	fast_sort fs(md);
	fs.sort();
	cout << "fast sort compares " << fs.number() << " times" << endl;
	auto & fs_restul = fs.ranking();
}

BOOST_AUTO_TEST_CASE(test_case3) {
	vector < vector<double> > data = { { 4,7 },{ 5,6 },{ 6,4 },{ 6,7 },{ 6,2 },{ 1,2 },{ 0,5 },{ 1,4, },{ 0,7 },{ 1,6 },{ 2,3 },{ 5,0 },{ 4,4 },{ 3,2 },{ 1,1 },{ 3,6 },{ 0,3 },{ 4,1 } };
	vector<int> no = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 };

	dominance_graph::graph dg(data);
	dg.quick_sort();
	dg.set_ranking();
	auto & dg_result = dg.ranking();

	vector<pair<int, vector<double>>> md;
	for (int i = 0; i < data.size(); ++i) {
		md.emplace_back(no[i], data[i]);
	}
	fast_sort fs(md);
	fs.sort();
	auto & fs_restul = fs.ranking();

	cout << (fs_restul == dg_result ? "match" : "not match") << endl;
	system("pause");
}

BOOST_AUTO_TEST_CASE(test_case) {
	random rand(0.5);
	int data_size(50);
	vector<vector<double>> data;
	vector<int> no;
	for (int i = 0; i < data_size; i++) {
		data.push_back(vector<double>({ rand.next(), rand.next() }));
		no.push_back(i);
	}

	dominance_graph::graph dg(data);
	dg.quick_sort();
	dg.set_ranking();
	auto & dg_result = dg.ranking();

	vector<pair<int, vector<double>>> md;
	for (int i = 0; i < data.size(); ++i) {
		md.emplace_back(no[i], data[i]);
	}
	fast_sort fs(md);
	fs.sort();
	auto & fs_restul = fs.ranking();

	cout << "quick sort compares " << dg.number() << " times" << endl;
	cout << "fast sort compares " << fs.number() << " times" << endl;
	cout << (fs_restul == dg_result ? "match" : "not match") << endl;
	system("pause");
}

BOOST_AUTO_TEST_SUITE_END()