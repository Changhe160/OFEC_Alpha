#include "../utility/catch.hpp"
#include "../run/custom_method.h"
#include "../core/algorithm/population.h"
#include "../core/algorithm/Solution.h"

#include<string>
#include<iostream>
using namespace ofec;



TEST_CASE("constrained optimization", "[CEC2017][DCNSGAII]")
{



	std::string pro_name = "COP_CEC2017_F";
	for (int pro_id(1); pro_id <= 28; ++pro_id) {
		char id1 = '0' + pro_id / 10;
		char id2 = '0' + pro_id % 10;
		std::string cur_pro_name = pro_name+id1+id2;
		
		std::cout << "cur problem names\t" << cur_pro_name << std::endl;

		ParameterMap params;
		params["problem name"] = cur_pro_name;
		params["number of variables"] = 3;
		params["number of objectives"] = 2;

		params["algorithm name"] = std::string("DCNSGAII-DE");
		params["population size"] = int(40);
		params["maximum evaluations"] = int(5000);
		int id_param = std::make_shared<const ParameterMap>(params);
		Problem *pro = Problem::generateByFactory(id_param, 0.1);
		pro->initialize();
		Algorithm *alg = Algorithm::generateByFactory(id_param, 0.1, 0.01, -1);
		alg.initialize();
		alg.run();

		DEL_ALG(alg);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

}