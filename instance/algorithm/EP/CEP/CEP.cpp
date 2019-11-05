#include "CEP.h"

namespace OFEC {
	CEP::CEP(param_map & v) : algorithm(v.at("algName")), m_pop(v.at("popSize")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void CEP::initialize() {
        m_pop.tau_prime() = 1 / (sqrt(2 / sqrt(m_pop.size())));
        m_pop.tau() = 1 / (sqrt(2 * m_pop.size()));
        m_pop.q() = 10;
		m_pop.initialize();
		m_pop.evaluate();
	}
	 
	void CEP::record() {
		if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
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

	void CEP::run_() {
		while (!terminating()) {
			m_pop.evolve();
		}
	}
}