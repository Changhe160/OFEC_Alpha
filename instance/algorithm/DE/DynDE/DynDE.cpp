#include "DynDE.h"

namespace OFEC {
	DynDE::DynDE(param_map &v) : algorithm(v.at("algName")), m_r_excl(v.at("exlRadius")),\
		m_multi_pop(v.at("popSize") / v.at("subPopSize"), v.at("subPopSize")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void DynDE::initialize() {
		for (size_t i = 0; i < m_multi_pop.size(); ++i) {
			m_multi_pop[i].initialize();
			m_multi_pop[i].evaluate();
		}
	}

	void DynDE::run_() {
		evaluation_tag tag = evaluation_tag::Normal;
		while (!terminating()) {
			for (size_t i = 0; i < m_multi_pop.size(); ++i) {
				if (!m_multi_pop[i].get_flag())
					tag = m_multi_pop[i].evolve();
				handle_evaluation_tag(tag);
			}
			if (tag == evaluation_tag::Terminate)
				break;
			//exclusion
			exclusion_check();
			for (size_t i = 0; i < m_multi_pop.size(); ++i) {
				if (m_multi_pop[i].get_flag()) {
					m_multi_pop[i].initialize();
					m_multi_pop[i].set_flag(false);
				}
			}
		}
	}

	void DynDE::exclusion_check() {
		for (size_t i = 0; i < m_multi_pop.size(); ++i) {
			for (size_t j = i + 1; j < m_multi_pop.size(); ++j) {
				if (m_multi_pop[i].get_flag() == false && m_multi_pop[j].get_flag() == false && m_multi_pop[i].best()[0]->variable_distance(*m_multi_pop[j].best()[0]) < m_r_excl) {
					if (m_multi_pop[i].best()[0]->dominate(*m_multi_pop[j].best()[0])) {
						m_multi_pop[j].set_flag(true);
					}
					else {
						m_multi_pop[i].set_flag(true);
					}
				}
			}
		}
	}

	void DynDE::record() {
		if (CONTINUOUS_CAST->has_tag(problem_tag::DOP)) {
			// ******* Dynamic Optimization ***********

		}
		else if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
			// ******* Multi-Modal Optimization *******
			size_t evals = CONTINUOUS_CAST->evaluations();
			size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
			size_t num_opt_known = CONTINUOUS_CAST->get_optima().number_objective();
			real peak_ratio = (real)num_opt_found / (real)num_opt_known;
			real success_rate = CONTINUOUS_CAST->solved() ? 1 : 0;
			measure::get_measure()->record(global::ms_global.get(), evals, peak_ratio, success_rate);
		}
		else if (CONTINUOUS_CAST->has_tag(problem_tag::GOP)) {
			// ******* Global Optimization ************
			size_t evals = CONTINUOUS_CAST->evaluations();
			real err = std::fabs(problem::get_sofar_best<solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
			measure::get_measure()->record(global::ms_global.get(), evals, err);
		}
	}
}
