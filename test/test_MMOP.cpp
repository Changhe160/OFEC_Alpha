#include <memory>

#include "../utility/catch.hpp"
#include "../run/include_problem.h"

using namespace OFEC;

TEST_CASE("CEC2013 Multi-Modal Benchmark Suit", "[CEC2013MMP]")
{
	global::ms_global = std::make_unique<global>(1, 0.5, 0.5);
	global::ms_arg["workingDir"] = "./";

	SECTION("F1") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F01>("MMOP_CEC2013_F01_five_uneven_peak_trap");
		global::ms_global->m_problem->initialize();
		size_t num_optima =  CONTINUOUS_CAST->get_optima().number_objective();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		REQUIRE(num_optima == 2);
		REQUIRE(val_optima == 200.0);
		solution<> s(1, 0, 1);
		s.initialize();
		s.evaluate(false, caller::Problem);
		global::ms_global->m_problem.reset();
	}

	SECTION("F2") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F02>("MMOP_CEC2013_F02_equal_maxima");
		global::ms_global->m_problem->initialize();
		size_t num_optima = CONTINUOUS_CAST->get_optima().number_objective();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		REQUIRE(num_optima == 5);
		REQUIRE(val_optima == 1.0);
		global::ms_global->m_problem.reset();
	}

	SECTION("F3") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F03>("MMOP_CEC2013_F03_uneven_de_maxima");
		global::ms_global->m_problem->initialize();
		size_t num_optima = CONTINUOUS_CAST->get_optima().number_objective();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		REQUIRE(num_optima == 1);
		REQUIRE(val_optima == 1.0);
		global::ms_global->m_problem.reset();
	}

	SECTION("F4") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F04>("MMOP_CEC2013_F4_himmenblau");
		global::ms_global->m_problem->initialize();
		size_t num_optima = CONTINUOUS_CAST->get_optima().number_objective();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		REQUIRE(num_optima == 4);
		REQUIRE(val_optima == 200.0);
		global::ms_global->m_problem.reset();
	}

	SECTION("F5") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F05>("MMOP_CEC2013_F5_six_hump_camel_back");
		global::ms_global->m_problem->initialize();
		size_t num_optima = CONTINUOUS_CAST->get_optima().number_objective();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		REQUIRE(num_optima == 2);
		REQUIRE(val_optima == 1.031628453);
		global::ms_global->m_problem.reset();
	}

	SECTION("F6(2D)") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F06>("MMOP_CEC2013_F6_shubert", 2);
		global::ms_global->m_problem->initialize();
		size_t num_optima = CONTINUOUS_CAST->get_optima().number_objective();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		REQUIRE(num_optima == 18);
		REQUIRE(val_optima == 186.7309088);
		global::ms_global->m_problem.reset();
	}

	SECTION("F7(2D)") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F07>("MMOP_CEC2013_F7_vincent", 2);
		global::ms_global->m_problem->initialize();
		size_t num_optima = CONTINUOUS_CAST->get_optima().number_objective();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		REQUIRE(num_optima == 36);
		REQUIRE(val_optima == 1.0);
		global::ms_global->m_problem.reset();
	}

	SECTION("F6(3D)") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F06>("MMOP_CEC2013_F6_shubert", 3);
		global::ms_global->m_problem->initialize();
		size_t num_optima = CONTINUOUS_CAST->get_optima().number_objective();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		REQUIRE(num_optima == 81);
		REQUIRE(val_optima == 2709.093505);
		global::ms_global->m_problem.reset();
	}

	SECTION("F7(3D)") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F07>("MMOP_CEC2013_F7_vincent", 3);
		global::ms_global->m_problem->initialize();
		size_t num_optima = CONTINUOUS_CAST->get_optima().number_objective();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		REQUIRE(num_optima == 216);
		REQUIRE(val_optima == 1.0);
		global::ms_global->m_problem.reset();
	}

	SECTION("F8") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F08>("MMOP_CEC2013_F8_modified_rastrigin", 2);
		global::ms_global->m_problem->initialize();
		size_t num_optima = CONTINUOUS_CAST->get_optima().number_objective();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		REQUIRE(num_optima == 12);
		REQUIRE(val_optima == -2.0);
		global::ms_global->m_problem.reset();
	}

	SECTION("F9") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F09>("MMOP_CEC2013_F9_composition1", 2);
		global::ms_global->m_problem->initialize();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		solution<> temp_sol(1, 2);
		for (size_t i = 0; i < CONTINUOUS_CAST->get_optima().number_variable(); ++i) {
			temp_sol.variable() = CONTINUOUS_CAST->get_optima().variable(i);
			temp_sol.evaluate();
			REQUIRE(val_optima == temp_sol.objective()[0]);
		}
		global::ms_global->m_problem.reset();
	}

	SECTION("F10") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F10>("MMOP_CEC2013_F10_composition2", 2);
		global::ms_global->m_problem->initialize();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		solution<> temp_sol(1, 2);
		for (size_t i = 0; i < CONTINUOUS_CAST->get_optima().number_variable(); ++i) {
			temp_sol.variable() = CONTINUOUS_CAST->get_optima().variable(i);
			temp_sol.evaluate();
			REQUIRE(val_optima == temp_sol.objective()[0]);
		}
		global::ms_global->m_problem.reset();
	}

	SECTION("F11") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F11>("MMOP_CEC2013_F11_composition3", 2);
		global::ms_global->m_problem->initialize();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		solution<> temp_sol(1, 2);
		for (size_t i = 0; i < CONTINUOUS_CAST->get_optima().number_variable(); ++i) {
			temp_sol.variable() = CONTINUOUS_CAST->get_optima().variable(i);
			temp_sol.evaluate();
			REQUIRE(val_optima == temp_sol.objective()[0]);
		}
		global::ms_global->m_problem.reset();
	}

	SECTION("F12") {
		global::ms_global->m_problem = std::make_unique<CEC2013_MMOP_F12>("MMOP_CEC2013_F12_composition4", 3);
		global::ms_global->m_problem->initialize();
		real val_optima = CONTINUOUS_CAST->get_optima().objective(0)[0];
		solution<> temp_sol(1, 2);
		for (size_t i = 0; i < CONTINUOUS_CAST->get_optima().number_variable(); ++i) {
			temp_sol.variable() = CONTINUOUS_CAST->get_optima().variable(i);
			temp_sol.evaluate();
			REQUIRE(val_optima == temp_sol.objective()[0]);
		}
		global::ms_global->m_problem.reset();
	}
}