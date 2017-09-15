/*boost unit test for multi-objective sorting*/

#define BOOST_TEST_MODULE MO_sort
#include <boost/test/unit_test.hpp>
#include "../utility/MO_sort/dominancegraph_insert_sort.h"
#include "../utility/MO_sort/fast_nondominated_sort.h"
#include "../utility/random/newran.h"

BOOST_AUTO_TEST_SUITE(MO_sort_test)

//BOOST_AUTO_TEST_CASE(test_case1) {
//	random rand(0.5);
//	int data_size(100);
//	vector<vector<double>> data;
//	for (int i = 0; i < data_size; i++) {
//		data.push_back(vector<double>({ rand.next(), rand.next() }));
//	}
//	dominance_graph::graph dg(data);
//	auto& dg_result = dg.update_ranking();
//	cout << "quick sort compares " << dg.number() << " times" << endl;
//	system("pause");
//
//}

BOOST_AUTO_TEST_CASE(test_case1) {

	random rand(0.5);
	int data_size(10000);
	vector<vector<double>> data;
	vector<int> no;
	for (int i = 0; i < data_size; i++) {
		data.push_back(vector<double>({ rand.next(), rand.next(), rand.next()}));
		no.push_back(i);
	}

	dominance_graph::graph dg(data);
	auto& dg_result = dg.update_ranking();

	vector<pair<int, vector<double>>> md;
	for (int i = 0; i < data.size(); ++i) {
		md.emplace_back(no[i], data[i]);
	}
	fast_sort fs(md);
	fs.sort();
	auto & fs_result = fs.ranking();

	cout << "quick sort compares " << dg.number() << " times" << endl;
	cout << "fast sort compares " << fs.number() << " times" << endl;
	cout << (fs_result == dg_result ? "match" : "not match") << endl;

	//cout << "dg:" << endl;
	//for (auto& x : dg_result)
	//	cout << x.first << " " << x.second << endl;
	//cout << "fs:" << endl;
	//for (auto& x : fs_result)
	//	cout << x.first << " " << x.second << endl;
	system("pause");
}

BOOST_AUTO_TEST_SUITE_END()