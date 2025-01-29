/********* Begin Register Information **********
{ "description": "Some examples of unit tests" }
*********** End Register Information **********/

#include "../utility/catch.hpp"
#include "../core/global.h"
#include <iostream>
#include "../core/problem/solution.h"
#include "../instance/algorithm/template/classic/de/population.h"

using namespace ofec;

TEST_CASE("exmaple", "[case1]") {
	ParameterMap params;
	params["problem name"] = std::string("BBOB_F01");
	params["number of variables"] = 2;

	SECTION("CASE problem") {
		int id_param = std::make_shared<const ParameterMap>(params);
		Problem *pro = Problem::generateByFactory(id_param, 0.1);
		pro->initialize();
		/* release memory  */
		DEL_PRO(pro);
	}

	SECTION("CASE solution") {
		int id_param = std::make_shared<const ParameterMap>(params);
		Problem *pro = Problem::generateByFactory(id_param, 0.1);
		pro->initialize();
		Solution<> sol(pro->numberObjectives(), pro->numberConstraints(), CAST_CONOP(pro)->numberVariables());

		Random *rnd = ADD_RND(0.1);
		sol.initialize(pro, rnd);
		sol.evaluate(pro, -1);

		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("CASE population") {
		int id_param = std::make_shared<const ParameterMap>(params);
		Problem *pro = Problem::generateByFactory(id_param, 0.1);
		pro->initialize();

		PopulationDE<> pop_de(20,pro);

		Random *rnd = ADD_RND(0.1);

		pop_de.initialize(pro, rnd);
		pop_de.evaluate(pro, -1);
		pop_de.evolve(pro, -1, rnd);

		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("CASE algorithm") {
		ParameterMap params;
		params["problem name"] = std::string("BBOB_F01");
		params["number of variables"] = 2;
		params["algorithm name"] = std::string("Canonical-DE");
		params["population size"] = 20;
		params["maximum evaluations"] = 1000;

		int id_param = std::make_shared<const ParameterMap>(params);

		Problem *pro = Problem::generateByFactory(id_param, 0.1);
		Algorithm *alg = Algorithm::generateByFactory(id_param, pro, 0.01, -1);

		pro->initialize();
		alg->initialize();
		alg->run();

		DEL_ALG(alg);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}
}

void runAlgExample(Algorithm *alg) {
	{
		std::unique_lock<std::mutex> lock(g_cout_mutex);
		std::cout << "------------ Start run: Alg ID " << alg << std::endl;
	}
	alg->run();
	{
		std::unique_lock<std::mutex> lock(g_cout_mutex);
		std::cout << "++++++++++++ Finish run: Alg ID " << alg << std::endl;
	}
}

TEST_CASE("example2", "[case2]") {
	ParameterMap params;
	params["problem name"] = std::string("BBOB_F01");
	params["number of variables"] = 2;
	params["algorithm name"] = std::string("Canonical-DE");
	params["population size"] = int(20);
	params["maximum evaluations"] = int(1000);
	int id_param1 = std::make_shared<const ParameterMap>(params);
	int id_pro1 = Problem::generateByFactory(id_param1, 0.1);
	GET_PRO(id_pro1)->initialize();
	int id_alg1 = Algorithm::generateByFactory(id_param1, 0.1, 0.01, -1);
	GET_ALG(id_alg1)->initialize();

	ParameterMap params2;
	params2["problem name"] = std::string("BBOB_F02");
	params2["number of variables"] = 2;
	params2["algorithm name"] = std::string("Canonical-DE");
	params2["population size"] = 20;
	params2["maximum evaluations"] = int(1000);
	int id_param2 = std::make_shared<const ParameterMap>(params);
	int id_pro2 = Problem::generateByFactory(id_param2, 0.1);
	GET_PRO(id_pro2)->initialize();
	int id_alg2 = Algorithm::generateByFactory(id_param2, 0.1, 0.01, -1);
	GET_ALG(id_alg2)->initialize();

	std::vector<std::thread> thrds;
	thrds.push_back(std::thread(runAlgExample, id_alg1));
	thrds.push_back(std::thread(runAlgExample, id_alg2));
	for (auto& it : thrds) it.join();

	DEL_ALG(id_alg1);
	DEL_PRO(id_pro1);
	DEL_PARAM(id_param1);
	DEL_ALG(id_alg2);
	DEL_PRO(id_pro2);
	DEL_PARAM(id_param2);
}

TEST_CASE("example3", "[case3]") {
	ParameterMap params;
	params["problem name"] = std::string("BBOB_F01");
	params["number of variables"] = 2;
	params["algorithm name"] = std::string("Canonical-DE");
	params["population size"] = int(20);
	params["maximum evaluations"] = int(1000);
	int num_runs(20);
	params["number of runs"] = num_runs;

	int id_param = std::make_shared<const ParameterMap>(params);
	int id_rcr = ADD_RCR(id_param);
	std::list<int> ids_algs, ids_pros;
	std::vector<std::thread> thrds;

	for (size_t i = 0; i < num_runs; i++) {
		Problem *pro = Problem::generateByFactory(id_param, Real(i + 1) / (num_runs + 1));
		Algorithm *alg = Algorithm::generateByFactory(id_param, pro, Real(i + 1) / (num_runs + 1), id_rcr);
		pro->initialize();
		alg->initialize();
		ids_pros.push_back(pro);
		ids_algs.push_back(alg);
		thrds.emplace_back(runAlgExample, alg);
	}
	for (auto& t : thrds)
		t.join();

	GET_RCR(id_rcr)->outputData(ids_pros, ids_algs);

	for (Algorithm *alg : ids_algs) DEL_ALG(alg);
	for (Problem *pro : ids_pros) DEL_PRO(pro);
}
