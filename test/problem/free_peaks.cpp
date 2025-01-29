#include "../../utility/catch.hpp"

using namespace ofec;

TEST_CASE("free peaks", "[FPs]") {
	ParameterMap params;
	params["problem name"] = std::string("free peaks");
	params["generation"] = std::string("read_file");
	params["file"] = std::string("sop/6_2d_s1");

	SECTION("initialize") {
		int id_param = std::make_shared<const ParameterMap>(params);
		Problem *pro = Problem::generateByFactory(id_param, 0.1);
		pro->initialize();
	}
}