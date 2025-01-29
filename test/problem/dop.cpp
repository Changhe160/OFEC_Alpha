#include "../utility/catch.hpp"
#include "../core/problem/solution.h"
#include "../instance/algorithm/dynamic/mqso/mqso.h"
#include "../instance/algorithm/dynamic/hmso/hmso.h"
#include "../instance/algorithm/dynamic/cpso/cpso.h"
#include "../instance/algorithm/dynamic/cpsor/cpsor.h"
#include "../instance/algorithm/dynamic/samo/samo.h"
#include "../instance/algorithm/dynamic/spso/spso.h"
#include "../instance/problem/continuous/dynamic/moving_peaks/moving_peak.h"

using namespace ofec;

void runAlg(Algorithm *alg) {
	alg.run();
}

TEST_CASE("moving peaks benchmark", "[DOP][Problem]")
{
	/*
		//moving_peak::moving_peak(const std::string &name, int rDimNumber,  int rNumPeaks, Real  rChangingRatio,  int fre, Real vlength, bool rFlagDimChange, \
	//	 bool rFlagNumPeakChange,  int peakNumChangeMode,  bool flagNoise,  bool flagTimelinkage) :

		global::ms_global->m_problem.reset(new moving_peak("DOP_moving_peaks", 5, 10, 1.0, 3, 1.0, false, true));

	*/




	ParameterMap params;
	params["problem name"] = std::string("Moving-Peaks");
	params["number of variables"] = 5;
	params["numPeak"] = 10;
	params["changePeakRatio"] = 1.0;
	params["changeFre"] = 5000;
	params["shiftLength"] = 1.0;
	params["flagNumDimChange"] = false;
	params["flagNumPeakChange"] = false;

	params["sample frequency"] = 5000000;
	params["algorithm name"] = std::string("CPSOR");
	//params["number of free populations"] = 10000;
	params["population size"] = 100;
	params["max subpop size"] = 4;
	//params["Separate Radius"] = 45.5;
	params["maximum evaluations"] = 500000;

	int num_runs = 1;
	int id_param = std::make_shared<const ParameterMap>(params);
	int id_rcr = ADD_RCR(id_param);
	Problem *pro = Problem::generateByFactory(id_param, 0.1);
	Algorithm *alg = Algorithm::generateByFactory(id_param, pro, 0.2, id_rcr);
	pro->initialize();
	alg.initialize();
	alg.run();




	//std::vector<std::thread> thrds;
	//std::list<int> ids_pros, ids_algs;
	//for (size_t i = 0; i < num_runs; i++) {
	//	Real seed = Real(i + 1.00) / (num_runs + 1);
	//	Problem *pro = Problem::generateByFactory(id_param, seed);
	//	Algorithm *alg = Algorithm::generateByFactory(id_param, pro, seed, id_rcr);
	//	pro->initialize();
	//	alg.initialize();
	//	ids_pros.push_back(pro);
	//	ids_algs.push_back(alg);
	//}

	//for (Algorithm *alg : ids_algs)
	//	thrds.emplace_back(runAlg, alg);
	//for (auto& t : thrds)
	//	t.join();

	//GET_RCR(id_rcr).outputData(ids_pros, ids_algs);
	
	DEL_PARAM(id_param);
	DEL_ALG(alg);
	DEL_PRO(pro);
	//for (Algorithm *alg : ids_algs) DEL_ALG(alg);
	//for (Problem *pro : ids_pros) DEL_PRO(pro);
	DEL_RCR(id_rcr);
}
//
//TEST_CASE("dynamic rotation peak benchmark", "[rotationDBG]")
//{
//	//argument: -b [rotationDBG] 
//	global::ms_global = std::unique_ptr<global>(new global(1, 0.5, 0.5));
//	global::ms_global->m_problem.reset(new rotation_DBG("DOP_rotationDBG", 5, 10, 3, dynamic::ChangeType::CT_Random, 1.0, true, false, 1, false, false));
//    global::ms_global->m_problem->initialize();
//
//    solution<> s(global::ms_global->m_problem->objective_size(), 0, global::ms_global->m_problem->variable_size());
//    s.initialize();
//
//	for (int i = 0; i < 5; ++i) {
//		if (evaluation_tag::Change_decision_memory == s.evaluate(false, caller::Problem)) {
//			s.variable().resize(global::ms_global->m_problem->variable_size());
//		}
//	}
//}
//
//TEST_CASE("dynamic composition benchmark", "[compositionDBG]")
//{
//	//argument: -b [compositionDBG] 
//	global::ms_global = std::unique_ptr<global>(new global(1, 0.5, 0.5));
//
//	SECTION("dimensional change") {
//		global::ms_global->m_problem.reset(new composition_DBG("DOP_compositionDBG", 5, 10, 3, dynamic::ChangeType::CT_Random, composition_DBG::ComDBGFuncID::COMDBG_ACKLEY, 1.0, true, false, 1, false, false));
//        global::ms_global->m_problem->initialize();
//
//        solution<> s(global::ms_global->m_problem->objective_size(), 0, global::ms_global->m_problem->variable_size());
//        s.initialize();
//
//		for (int idx = 0; idx < static_cast<int>(composition_DBG::ComDBGFuncID::COMDBG_End); ++idx) {
//			dynamic_cast<composition_DBG*>(global::ms_global->m_problem.get())->set_fun_idx(static_cast<composition_DBG::ComDBGFuncID>(idx));
//			for (int i = 0; i < 5; ++i) {
//				if (evaluation_tag::Change_decision_memory == s.evaluate(false, caller::Problem)) {
//					s.variable().resize(global::ms_global->m_problem->variable_size());
//				}
//			}
//		}
//		global::ms_global->m_problem.reset();
//	}
//
//	SECTION("random change") {
//		global::ms_global->m_problem.reset(new composition_DBG("DOP_compositionDBG", 5, 10, 3, dynamic::ChangeType::CT_Random, composition_DBG::ComDBGFuncID::COMDBG_ACKLEY, 1.0, false, false, 1, false, false));
//        global::ms_global->m_problem->initialize();
//
//        solution<> s(global::ms_global->m_problem->objective_size(), 0, global::ms_global->m_problem->variable_size());
//        s.initialize();
//
//		for (int idx = 0; idx < static_cast<int>(composition_DBG::ComDBGFuncID::COMDBG_End); ++idx) {
//			dynamic_cast<composition_DBG*>(global::ms_global->m_problem.get())->set_fun_idx(static_cast<composition_DBG::ComDBGFuncID>(idx));
//			for (int i = 0; i < 5; ++i) {
//                s.evaluate(false, caller::Problem);
//			}
//		}
//		global::ms_global->m_problem.reset();
//	}
//
//	SECTION("number of peaks change") {
//		global::ms_global->m_problem.reset(new composition_DBG("DOP_compositionDBG", 5, 10, 3, dynamic::ChangeType::CT_Random, composition_DBG::ComDBGFuncID::COMDBG_ACKLEY, 1.0, false, 1, 1, false, false));
//        global::ms_global->m_problem->initialize();
//
//        solution<> s(global::ms_global->m_problem->objective_size(), 0, global::ms_global->m_problem->variable_size());
//        s.initialize();
//
//		for (int idx = 0; idx < static_cast<int>(composition_DBG::ComDBGFuncID::COMDBG_End); ++idx) {
//			dynamic_cast<composition_DBG*>(global::ms_global->m_problem.get())->set_fun_idx(static_cast<composition_DBG::ComDBGFuncID>(idx));
//			for (int i = 0; i < 5; ++i) {
//                s.evaluate(false, caller::Problem);
//			}
//		}
//		global::ms_global->m_problem.reset();
//	}
//
//}