/*boost unit test for multi-objective non-dominated sorting*/

#define BOOST_TEST_MODULE nondominated_sorting
#include <boost/test/unit_test.hpp>
#include "../utility/nondominated_sorting/d_graph.h"
#include "../utility/nondominated_sorting/NSGA2.h"
#include "../utility/nondominated_sorting/preSorting.h"
#include "../utility/random/newran.h"
#include <time.h>

BOOST_AUTO_TEST_SUITE(nondominated_sorting_test)

//BOOST_AUTO_TEST_CASE(test_case1) {
//	random rand(0.5);
//	int data_size(10);
//	vector<vector<double>> data;
//	for (int i = 0; i < data_size; i++) {
//		data.push_back(vector<double>({ rand.next() }));
//	}
//	for (auto& x : data)
//		cout << x[0] << " ";
//	cout << endl;
//	NSD::preSorting(data, false);
//	//NSD::preSorting(data);
//	for (auto& x : data)
//		cout << x[0] << " ";
//	cout << endl;
//	system("pause");
//}

BOOST_AUTO_TEST_CASE(test_case2) {

	time_t start(0), end(0);

	random rand(0.1);
	int data_size(1000);
	vector<vector<double>> data;
	vector<int> no;
	vector<pair<int, vector<double>>> md;
	for (int i = 0; i < data_size; i++) {
		data.push_back(vector<double>({ rand.next(), rand.next(), rand.next(), rand.next() }));
	}

	//NSD::preSorting(data, false);

	for (int i = 0; i < data_size; i++) {
		no.push_back(i);
		md.emplace_back(no[i], data[i]);
	}

	cout << "Dominance graph Non-dominated sorting:" << endl;
	time(&start);
	NDS::dg_graph<double> dg(data);
	auto& dg_result = dg.ranking();
	time(&end);
	cout << "Compares " << dg.number() << " times" << endl;
	cout << "Cost " << (end - start) << " seconds" << endl << endl;

	cout << "NSGA¢ò Non-dominated sorting:" << endl;
	time(&start);
	NDS::fast_sort fs(md);
	fs.sort();
	auto & fs_result = fs.ranking();
	time(&end);
	cout << "Compares " << fs.number() << " times" << endl;
	cout << "cost " << (end - start) << " seconds" << endl << endl;

	cout << (fs_result == dg_result ? "match" : "not match") << endl << endl;

	system("pause");
}

BOOST_AUTO_TEST_SUITE_END()