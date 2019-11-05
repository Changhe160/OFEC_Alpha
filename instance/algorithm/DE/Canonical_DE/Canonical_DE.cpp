#include "./Canonical_DE.h"


namespace OFEC {
	Canonical_DE::Canonical_DE(param_map& v) : algorithm(v.at("algName")), m_pop(v.at("popSize")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void Canonical_DE::initialize() {
		m_pop.initialize();
		m_pop.evaluate();
	}

	void Canonical_DE::run_() {
		while (!terminating()) {
			m_pop.evolve();
		}
	}

	void Canonical_DE::record() {
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

	DE_rand_1::DE_rand_1(param_map& v) : Canonical_DE(v) {}

	void DE_rand_1::initialize() {
		m_pop.set_parameter(0.5, 0.6);
		m_pop.set_mutation_strategy(DE::mutation_strategy::rand_1);
		Canonical_DE::initialize();
	}

	DE_best_2::DE_best_2(param_map& v) : Canonical_DE(v) {}

	void DE_best_2::initialize() {
		m_pop.set_mutation_strategy(DE::mutation_strategy::best_2);
		Canonical_DE::initialize();
	}

	DE_nrand_1::DE_nrand_1(param_map& v) : Canonical_DE(v) {}

	void DE_nrand_1::initialize() {
		m_pop.set_parameter(0.9, 0.5);
		m_pop.set_mutation_strategy(DE::mutation_strategy::nrand_1);
		Canonical_DE::initialize();
	}
}