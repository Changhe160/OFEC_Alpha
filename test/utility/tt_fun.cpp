#include "../utility/catch.hpp"
#include"../utility/elementary_function.h"
#include "../utility/functional.h"
#include"../core/global.h"
#include<iostream>
#include "../utility/nondominated_sorting/fast_sort.h"
using namespace std;
using namespace ofec;


TEST_CASE("functional", "[functional]")
{


	Uniform u(0.4);



	//std::vector<double> datas(10);
	//for (auto& it : datas) {
	//	it = u.nextNonStd<double>(0, 5);
	//}

	//datas.emplace_back(6);
	//u.shuffle(datas.begin(), datas.end());


	////auto com2 = std::bind(objectiveCompare, _1, _2, OptimizeMode::Maximize);
	//auto best_idx = calBestIdx<double>(datas, com);

	//int stop = -1;




	std::vector<std::vector<double>> datas(10,std::vector<double>(2,0));
	for (auto& it : datas) {
		for (auto& it2 : it) {
			it2 = u.nextNonStd<double>(0, 5);
		}
	}

	datas.emplace_back(std::vector<double>{ 5,6 });
	datas.emplace_back(std::vector<double>{ 6,5 });
	u.shuffle(datas.begin(), datas.end());

	std::function<ofec::Dominance(const std::vector<double>& a, const std::vector<double>& b)> com = [](const std::vector<double>& a, const std::vector<double>& b) {
		return objectiveCompare<double>(a, b, OptimizeMode::Maximize);
	};

	//auto com2 = std::bind(objectiveCompare, _1, _2, OptimizeMode::Maximize);
	auto best_idx = calBestIdx<std::vector<double>>(datas, com);

	int stop = -1;

	std::vector<std::vector<double>> best_datas;
	for (auto& it : best_idx) {
		best_datas.push_back(datas[it]);
	}

	
	std::vector<double> a = { 5.5,5.5 };
	updateBestSol(best_datas, a,com);
	std::vector<double> b = { 7,7 };
	updateBestSol(best_datas, b,com);
	

	

}



TEST_CASE("utility", "[elementary_function]")
{


	std::pair<double, double> from_x_range = { 0,1 };
	std::pair<double, double> to_x_range = { 0,1 };
	std::pair<double, double> to_y_range = { 0,1 };
	int sample_fre = 100;
	auto id=ADD_RND(0.1);
	random_fun test_fun(id,sample_fre,from_x_range,to_x_range,to_y_range);
	test_fun.initialize();

	const int max_n = 10000;
	for (int id(0); id < max_n; ++id) {
		cout << "id\t" << id << "\t" << test_fun.get_value(id *1.0/ max_n) << endl;
	}

}