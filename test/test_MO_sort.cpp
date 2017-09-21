/*boost unit test for multi-objective sorting*/

#define BOOST_TEST_MODULE nondominated_sorting
#include <boost/test/unit_test.hpp>
#include "../utility/nondominated_sorting/d_graph.h"
#include "../utility/nondominated_sorting/NSGA2.h"
#include "../utility/random/newran.h"

BOOST_AUTO_TEST_SUITE(nondominated_sorting_test)

BOOST_AUTO_TEST_CASE(test_case1) {
	random rand(0.5);
	int data_size(100);
	vector<vector<double>> data;
	vector<int> no;
	vector<pair<int, vector<double>>> md;
	for (int i = 0; i < data_size; i++) {
		data.push_back(vector<double>({ rand.next(), rand.next() }));
		no.push_back(i);
		md.emplace_back(no[i], data[i]);
	}

	fast_sort fs(md);
	fs.sort();
	auto & fs_result = fs.ranking();

	cout << "fast sort compares " << fs.number() << " times" << endl;
	system("pause");
}

BOOST_AUTO_TEST_CASE(test_case2) {
	random rand(0.5);
	int data_size(100);
	vector<vector<double>> data;
	vector<int> no;
	vector<pair<int, vector<double>>> md;
	for (int i = 0; i < data_size; i++) {
		data.push_back(vector<double>({ rand.next(), rand.next() }));
		no.push_back(i);
		md.emplace_back(no[i], data[i]);
	}

	d_graph<double> dg(md);
	dg.update_ranking();
	auto& dg_result = dg.ranking();

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