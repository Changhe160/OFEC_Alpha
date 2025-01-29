#include <iomanip>
#include "../utility/catch.hpp"
#include "../run/custom_method.h"
#include "../core/algorithm/population.h"
#include "../core/algorithm/Solution.h"

using namespace ofec;

TEST_CASE("MOP", "[OOMOP1]")
{


	ParameterMap params;
	params["problem name"] = std::string("MOP_OOMOP1");
	params["number of objectives"] = 2;
	params["algorithm name"] = std::string("NSGAII-SBX");
	params["population size"] = int(40);
	params["maximum evaluations"] = int(2000);

	std::vector<int> pri_var = { 2,2 };
	params["vector of private variables"] = pri_var;
	params["number of public variables"] = 2;
	std::vector<int> pri_peak_num = { 2,2 };//number of obj
	params["vector of private peaks"] = pri_peak_num;
	params["number of variables"] = 6;
	std::vector<Real> pri_peak_height_rate = { 0.5,0.5 };//number of obj
	params["private peak height ratio"] = pri_peak_height_rate;
	std::vector<Real> pri_peak_slope_range = { 5,10 };
	params["private peak slope range"] = pri_peak_slope_range;
	std::vector<Real> pri_dim_norm = { 2,2 };//number of obj
	params["private dim norm"] = pri_dim_norm;
	std::vector<Real> pub_dim_norm = { 2,2 };//number of pub dim
	params["public dim norm"] = pub_dim_norm;
	params["number of PS shapes"] = 2;
	params["number of global PS shapes"] = 1;
	std::vector<Real> ps_span = { 0.2};//number of global ps
	params["vector of global PS span"] = ps_span;
	std::vector<Real> ps_decay_rate = { 0.8};//number of global ps
	params["PS decay rate"] = ps_decay_rate;
	std::vector<Real> ps_radiate_slope = {200, 100};//number of ps
	params["PS radiate slope"] = ps_radiate_slope;
	std::vector<int> ps_type = {0};//number of global ps
	params["global PS shape type"] = ps_type;
	params["PF shape type"] = 0;
	

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
