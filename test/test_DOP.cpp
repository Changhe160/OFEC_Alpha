#include "../utility/catch.hpp"
#include "../core/algorithm/solution.h"
#include "../run/include_problem.h"

using namespace OFEC;

TEST_CASE("moving peaks benchmark", "[MPB]")
{
	//argument: -b [MPB] 
	global::ms_global = std::unique_ptr<global>(new global(1, 0.5, 0.5));
	global::ms_global->m_problem.reset(new moving_peak("DOP_moving_peaks", 5, 10, 1.0, 3, 1.0, false, true));
	global::ms_global->m_problem->initialize();

    solution<> s(global::ms_global->m_problem->objective_size(), 0, global::ms_global->m_problem->variable_size());
    s.initialize();
	for (int i = 0; i < 5; ++i)
		s.evaluate(false, caller::Problem);
}

TEST_CASE("dynamic rotation peak benchmark", "[rotationDBG]")
{
	//argument: -b [rotationDBG] 
	global::ms_global = std::unique_ptr<global>(new global(1, 0.5, 0.5));
	global::ms_global->m_problem.reset(new rotation_DBG("DOP_rotationDBG", 5, 10, 3, dynamic::ChangeType::CT_Random, 1.0, true, false, 1, false, false));
    global::ms_global->m_problem->initialize();

    solution<> s(global::ms_global->m_problem->objective_size(), 0, global::ms_global->m_problem->variable_size());
    s.initialize();

	for (int i = 0; i < 5; ++i) {
		if (evaluation_tag::Change_dimension == s.evaluate(false, caller::Problem)) {
			s.variable().resize(global::ms_global->m_problem->variable_size());
		}
	}
}

TEST_CASE("dynamic composition benchmark", "[compositionDBG]")
{
	//argument: -b [compositionDBG] 
	global::ms_global = std::unique_ptr<global>(new global(1, 0.5, 0.5));

	SECTION("dimensional change") {
		global::ms_global->m_problem.reset(new composition_DBG("DOP_compositionDBG", 5, 10, 3, dynamic::ChangeType::CT_Random, composition_DBG::ComDBGFuncID::COMDBG_ACKLEY, 1.0, true, false, 1, false, false));
        global::ms_global->m_problem->initialize();

        solution<> s(global::ms_global->m_problem->objective_size(), 0, global::ms_global->m_problem->variable_size());
        s.initialize();

		for (int idx = 0; idx < static_cast<int>(composition_DBG::ComDBGFuncID::COMDBG_End); ++idx) {
			dynamic_cast<composition_DBG*>(global::ms_global->m_problem.get())->set_fun_idx(static_cast<composition_DBG::ComDBGFuncID>(idx));
			for (int i = 0; i < 5; ++i) {
				if (evaluation_tag::Change_dimension == s.evaluate(false, caller::Problem)) {
					s.variable().resize(global::ms_global->m_problem->variable_size());
				}
			}
		}
		global::ms_global->m_problem.reset();
	}

	SECTION("random change") {
		global::ms_global->m_problem.reset(new composition_DBG("DOP_compositionDBG", 5, 10, 3, dynamic::ChangeType::CT_Random, composition_DBG::ComDBGFuncID::COMDBG_ACKLEY, 1.0, false, false, 1, false, false));
        global::ms_global->m_problem->initialize();

        solution<> s(global::ms_global->m_problem->objective_size(), 0, global::ms_global->m_problem->variable_size());
        s.initialize();

		for (int idx = 0; idx < static_cast<int>(composition_DBG::ComDBGFuncID::COMDBG_End); ++idx) {
			dynamic_cast<composition_DBG*>(global::ms_global->m_problem.get())->set_fun_idx(static_cast<composition_DBG::ComDBGFuncID>(idx));
			for (int i = 0; i < 5; ++i) {
                s.evaluate(false, caller::Problem);
			}
		}
		global::ms_global->m_problem.reset();
	}

	SECTION("number of peaks change") {
		global::ms_global->m_problem.reset(new composition_DBG("DOP_compositionDBG", 5, 10, 3, dynamic::ChangeType::CT_Random, composition_DBG::ComDBGFuncID::COMDBG_ACKLEY, 1.0, false, 1, 1, false, false));
        global::ms_global->m_problem->initialize();

        solution<> s(global::ms_global->m_problem->objective_size(), 0, global::ms_global->m_problem->variable_size());
        s.initialize();

		for (int idx = 0; idx < static_cast<int>(composition_DBG::ComDBGFuncID::COMDBG_End); ++idx) {
			dynamic_cast<composition_DBG*>(global::ms_global->m_problem.get())->set_fun_idx(static_cast<composition_DBG::ComDBGFuncID>(idx));
			for (int i = 0; i < 5; ++i) {
                s.evaluate(false, caller::Problem);
			}
		}
		global::ms_global->m_problem.reset();
	}

}