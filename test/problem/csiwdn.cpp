#include "../utility/catch.hpp"

#include "../instance/problem/realworld/csiwdn/csiwdn.h"
#include <fstream>

using namespace ofec;

TEST_CASE("CSIWDN", "[CSIWDN]") 
{

	ParameterMap params;
	params["problem name"] = std::string("CSIWDN");
	params["dataFile1"] = std::string("Net2");
	params["dataFile2"] = std::string("case1");
	params["dataFile3"] = std::string("case11");
	params["use LSTM"] = false;

	//SECTION("CASE EPANET") {
	//	int id_param = std::make_shared<const ParameterMap>(params);
	//	Problem *pro = Problem::generateByFactory(id_param, 0.1);
	//	pro->initialize();
	//}

	//SECTION("CASE LSTM") {
	//	int id_param = std::make_shared<const ParameterMap>(params);
	//	Problem *pro = Problem::generateByFactory(id_param, 0.1);
	//	pro->initialize();
	//	CAST_CSIWDN(pro)->setUseLSTM(true);

	//	Solution<VarCSIWDN> sol(1, 0, CAST_CSIWDN(pro)->numberSource());

	//	Random *rnd = ADD_RND(0.5);
	//	sol.initialize(pro, rnd);
	//	sol.evaluate(pro, -1);

	//	CAST_CSIWDN(pro)->setAlgorithmStart();
	//	sol.initialize(pro, rnd);
	//	for (size_t i = 0; i < 10000; ++i)
	//		sol.evaluate(pro, -1);

	//	DEL_PRO(pro);
	//}

	SECTION("CASE EPANET") {


		int id_param = std::make_shared<const ParameterMap>(params);
		Problem *pro = Problem::generateByFactory(id_param, 0.1);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();


		size_t num_sols = 100;

		std::vector<Solution<VarCSIWDN>> sols(num_sols, Solution<VarCSIWDN>(1, 0, CAST_CSIWDN(pro)->numberSource()));
		for (size_t i = 0; i < num_sols; ++i)
			sols[i].initialize(pro, rnd);

		std::vector<Real> objs(num_sols);
		std::vector<std::vector<bool>> epa_domin(num_sols, std::vector<bool>(num_sols));
		for (size_t i = 0; i < num_sols; ++i)
			sols[i].evaluate(pro, -1);
		for (size_t i = 0; i < num_sols; i++) {
			for (size_t j = i+1; j < num_sols; j++) {
				epa_domin[i][j] = sols[i].dominate(sols[j], pro);
			}
		}
		for (size_t i = 0; i < num_sols; ++i)
			objs[i] = sols[i].objective(0);

		std::vector<std::vector<bool>> lstm_domin(num_sols, std::vector<bool>(num_sols));
		CAST_CSIWDN(pro)->setUseLSTM(true);
		for (size_t i = 0; i < num_sols; ++i)
			sols[i].evaluate(pro, -1);
		for (size_t i = 0; i < num_sols; i++) {
			for (size_t j = i + 1; j < num_sols; j++) {
				lstm_domin[i][j] = sols[i].dominate(sols[j], pro);
			}
		}

		int eql_count = 0, total_count = 0;
		for (size_t i = 0; i < num_sols; ++i) {
			for (size_t j = i+1; j < num_sols; ++j) {
				total_count++;
				if (epa_domin[i][j] == lstm_domin[i][j]) {
					eql_count++;
				}
				else {
					std::cout << "i:" << i << ", j:" << j << "   ";
				}
			}
			std::cout << std::endl;
		}
		std::cout << "ratio:" << double(eql_count)/ total_count << std::endl;

		Solution<VarCSIWDN> optima(1, 0, CAST_CSIWDN(pro)->numberSource());
		optima.variable() = CAST_CSIWDN(pro)->optima().variable(0);
		optima.evaluate(pro, -1);
		std::cout << "optima, obj_lstm:" << optima.objective(0) << std::endl;
		for (size_t i = 0; i < num_sols; ++i) {
			std::cout << "i:" << i << ", obj_epa:"<<objs[i] << ", obj_lstm:" << sols[i].objective(0) << std::endl;
		}
	}
	

	//SECTION("CASE algorithm") {
	//	
	//	params["algorithm name"] = std::string("AMP-GL-SaDE");
	//	params["subpopulation size"] = 20;
	//	params["number of SubPopulations"] = 10;
	//	params["alpha"] = 2;
	//	params["beta"] = 2;
	//	params["maximum evaluations"] = 200000;

	//	int id_param = std::make_shared<const ParameterMap>(params);

	//	Problem *pro = Problem::generateByFactory(id_param, 0.1);
	//	Algorithm *alg = Algorithm::generateByFactory(id_param, pro, 0.01, -1);

	//	pro->initialize();
	//	alg.initialize();
	//	alg.run();


	//	DEL_ALG(alg);
	//	DEL_PRO(pro);
	//	DEL_PARAM(id_param);
	//}
}
