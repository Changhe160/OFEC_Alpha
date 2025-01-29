#include "../utility/catch.hpp"
#include"../core/global.h"

#include<iostream>
using namespace std;
using namespace ofec;

// this is a


TEST_CASE("NBN", "[NBN_sample_alg]")
{

	ParameterMap params;
	params["problem name"] = std::string("CSIWDN");
	params["dataFile1"] = std::string("Net2_97_case6");
	params["dataFile2"] = std::string("Net2");
	params["number of variables"] = 2;
	params["algorithm name"] = std::string("VISUAL_NBN_SAMPLE");
	params["population size"] = 20;
	params["maximum evaluations"] = 1000000;
	int id_param = std::make_shared<const ParameterMap>(params);
	Problem *pro = Problem::generateByFactory(id_param, 0.1);
	Random *rnd = ADD_RND(0.5);
	pro->initialize();


	
	//int id_rcr = ADD_RCR(id_param);
	Algorithm *alg = Algorithm::generateByFactory(id_param, pro, 0.2, -1);

	alg.initialize();
	alg.run();



	DEL_PRO(pro);
	DEL_RND(rnd);
}


