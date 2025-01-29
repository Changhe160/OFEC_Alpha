#include "../utility/catch.hpp"
#include "../run/include_problem.h"

#include "../core/problem/solution.h"

using namespace ofec;

TEST_CASE("CEC2013 Multi-Modal Benchmark Suit", "[CEC2013][MMOP]") {

	ParameterMap v;

	SECTION("F1") {
		v["problem name"] = std::string("MMOP_CEC2013_F01");
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 2);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 200.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F2") {
		v["problem name"] = std::string("MMOP_CEC2013_F02");
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 5);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 1.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F3") {
		v["problem name"] = std::string("MMOP_CEC2013_F03");
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 1);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 1.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F4") {
		v["problem name"] = std::string("MMOP_CEC2013_F04");
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 4);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 200.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F5") {
		v["problem name"] = std::string("MMOP_CEC2013_F05");
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 2);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 1.031628453) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.5);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F6(2D)") {
		v["problem name"] = std::string("MMOP_CEC2013_F06");
		v["number of variables"] = 2;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 18);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 186.7309088) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.5);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F7(2D)") {
		v["problem name"] = std::string("MMOP_CEC2013_F07");
		v["number of variables"] = 2;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 36);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 1.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.2);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F6(3D)") {
		v["problem name"] = std::string("MMOP_CEC2013_F06");
		v["number of variables"] = 3;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 81);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 2709.093505) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.5);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F7(3D)") {
		v["problem name"] = std::string("MMOP_CEC2013_F07");
		v["number of variables"] = 3;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 216);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 1.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.2);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F8") {
		v["problem name"] = std::string("MMOP_CEC2013_F08");
		v["number of variables"] = 3;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 12);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - (-2.0)) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F9") {
		v["problem name"] = std::string("MMOP_CEC2013_F09");
		v["number of variables"] = 2;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 6);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 0.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F10") {
		v["problem name"] = std::string("MMOP_CEC2013_F10");
		v["number of variables"] = 2;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 8);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 0.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F11(2D)") {
		v["problem name"] = std::string("MMOP_CEC2013_F11");
		v["number of variables"] = 2;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 6);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 0.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F11(3D)") {
		v["problem name"] = std::string("MMOP_CEC2013_F11");
		v["number of variables"] = 3;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 6);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 0.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F12(3D)") {
		v["problem name"] = std::string("MMOP_CEC2013_F12");
		v["number of variables"] = 3;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 8);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 0.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F11(5D)") {
		v["problem name"] = std::string("MMOP_CEC2013_F11");
		v["number of variables"] = 5;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 6);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 0.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F12(5D)") {
		v["problem name"] = std::string("MMOP_CEC2013_F12");
		v["number of variables"] = 5;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 8);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 0.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F11(10D)") {
		v["problem name"] = std::string("MMOP_CEC2013_F11");
		v["number of variables"] = 10;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 6);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 0.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F12(10D)") {
		v["problem name"] = std::string("MMOP_CEC2013_F12");
		v["number of variables"] = 10;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 8);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 0.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}

	SECTION("F12(20D)") {
		v["problem name"] = std::string("MMOP_CEC2013_F12");
		v["number of variables"] = 20;
		int id_param = std::make_shared<const ParameterMap>(v);
		Problem *pro = Problem::generateByFactory(id_param, 0.5);
		Random *rnd = ADD_RND(0.5);
		pro->initialize();
		size_t num_optima = pro->optima().number();
		REQUIRE(num_optima == 8);
		Solution<> s(1, 0, CAST_CONOP(pro)->numberVariables());
		for (size_t i = 0; i < num_optima; ++i) {
			s.variable() = pro->optima().variable(i);
			s.evaluate(pro, -1, false);
			REQUIRE(fabs(s.objective(0) - 0.0) < 1e-5);
		}
		Real niche_radius = CAST_CONOP(pro)->variableNicheRadius();
		REQUIRE(niche_radius == 0.01);
		DEL_RND(rnd);
		DEL_PRO(pro);
		DEL_PARAM(id_param);
	}
}
