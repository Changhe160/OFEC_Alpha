#include "../utility/catch.hpp"
#include "../run/custom_method.h"
#include "../core/algorithm/population.h"
#include "../core/algorithm/Solution.h"

#include "../instance/algorithm/visualize/sample_nearest_better_network.h"
#include "../utility/catch.hpp"
#include"../core/global.h"

#include<iostream>
#include "../core/problem/solution.h"
#include "../instance/algorithm/template/classic/de/population.h"
#include "../utility/function/custom_function.h"
#include "../instance/algorithm/visualize/dynamic_sample_algorithm.h"
#include "../instance/problem/combination/one_max/one_max.h"

using namespace std;
using namespace ofec;


//#include "../utility/random/newran.h"
//#include "../utility/nbn_visualization/nbn_visualization_data.h"
//int main(int argc, char* argv[]) {
//
//	Random rand(0.3);
//	ofec::NBN_Data data1;
//	ofec::NBN_Data data2;
//	data1.generateRandom(1000, rnd);
//
//	std::string filepath = std::string(OFEC_DIR) + "/1.txt";
//	if (data1.output(filepath,30)) {
//		std::cout << "output ok\t" << filepath << std::endl;
//	}
//	else std::cout << "output error\t" << filepath << std::endl;
//	
//	
//	if (data2.input(filepath)) {
//		std::cout << "input ok\t" << filepath << std::endl;
//	}
//	else {
//		std::cout << "input ERROR\t" << filepath << std::endl;
//	}
//
//	if (!ofec::NBN_Data::judgeEqual(data1, data2)) {
//		std::cout << "error" << std::endl;
//	}
//	else std::cout << "yes" << std::endl;
//}



TEST_CASE("NBN_COM_ONE_MAX", "[NBN][ONE_MAX]") {
	


	ParameterMap params;
	params["problem name"] = std::string("OneMax");
	params["number of variables"] = 100;

	int id_param = std::make_shared<const ParameterMap>(params);
	Problem *pro = Problem::generateByFactory(id_param, 0.1);
	Random *rnd = ADD_RND(0.5);
	pro->initialize();




	DEL_PRO(pro);
	DEL_RND(rnd);
	//params[]
}

TEST_CASE("NBN2", "[NBN_CASE1]")
{


	ParameterMap params;
	params["problem name"] = std::string("BBOB_F01");
	params["number of variables"] = 2;
}

//
//TEST_CASE("NBN alg", "[NBN_Alg]")
//{
//
//
//	ParameterMap params;
//	params["problem name"] = std::string("BBOB_F01");
//	params["number of variables"] = 2;
//	int id_param = std::make_shared<const ParameterMap>(params);
//	Problem *pro = Problem::generateByFactory(id_param, 0.1);
//	Random *rnd = ADD_RND(0.5);
//	pro->initialize();
//
//	std::function<void(Solution<>& sol, Problem *pro)> eval_fun =
//		[](Solution<>& sol, Problem *pro) {
//		using namespace ofec;
//		sol.evaluate(pro, -1, false);
//		ofec::Real pos = (pro->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;
//		sol.setFitness(pos * sol.objective(0));
//	};
//
//	Sample2D_Graph_Algorithm sample_alg;
//	sample_alg.init(pro, eval_fun);
//	sample_alg.calculate();
//	sample_alg.getTree();
//
//
//	DEL_PRO(pro);
//	DEL_RND(rnd);
//}
//
//
//
//
//TEST_CASE("NBN", "[Nearest_better_network][Sample_Algorithm]") {
//	using namespace ofec;
//
//
//	ParameterMap params;
//	params["problem name"] = std::string("BBOB_F01");
//	params["number of variables"] = 2;
//	int id_param = std::make_shared<const ParameterMap>(params);
//	Problem *pro = Problem::generateByFactory(id_param, 0.1);
//	Random *rnd = ADD_RND(0.5);
//	pro->initialize();
//
//
//	ofec::SampleNearestBetterNetwork sample_alg;
//
//	// std::vector<std::shared_ptr<SolutionType>>& sols
//
//	std::vector<std::unique_ptr<Solution<>>> sols(2000);
//
//	std::function<void(Solution<>& sol, Problem *pro)> eval_fun =
//		[](Solution<>& sol, Problem *pro) {
//		using namespace ofec;
//		sol.evaluate(pro, -1, false);
//		ofec::Real pos = (pro->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;
//		sol.setFitness(pos * sol.objective(0));
//	};
//
//	UTILITY::generateSamplesEval<Solution<>, std::unique_ptr>(
//		sols, rnd, pro, eval_fun);
//
//
//	//SampleNearestBetterNetwork nbn;
//	//nbn.initialize(rnd, pro);
//	//nbn.setEvalFun(eval_fun);
//	//
//	//int initSolNum = 2e3;
//	//nbn.initRandomSol(initSolNum);
//
//	//
//
//
//	//SampleNearestBetterNetworkRecord nbn_record;
//	//nbn.outputData(nbn_record);
//	//
//	//SampleNearestBetterNetwork nbn2;
//	//nbn2.insertData(nbn_record);
//
//	//
//
//	//SampleNearestBetterNetworkRecord nbn_record2;
//	//
//	//nbn2.outputData(nbn_record2);
//
//	//if (nbn_record.isEqualNetwork(nbn_record2)) {
//	//	std::cout << "ok\t" << std::endl;
//	//}
//
//
//	//sample_alg.initialize(rnd, pro);
//	//sample_alg.initRandomSol(1000);
//
//	DEL_PRO(pro);
//	DEL_RND(rnd);
//	
//}