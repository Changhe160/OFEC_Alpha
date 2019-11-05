#include "CRDE.h"

namespace OFEC {

	CRDE_pop::CRDE_pop(size_t size_pop) : DE::population<DE::individual>(size_pop) {}

	evaluation_tag CRDE_pop::evolve() {
		if (m_inds.size() < 5) {
			throw myexcept("the population size cannot be smaller than 5@DE::CRDE::evolve()");
		}
		evaluation_tag tag = evaluation_tag::Normal;
		for (size_t i = 0; i < size(); ++i) {
			mutate(i);
			m_inds[i]->recombine(m_CR, m_recombine_strategy);
			tag = m_inds[i]->trial().evaluate();
			if (tag == evaluation_tag::Terminate) return tag;
			int idx = nearest_neighbour(i).begin()->second;
			if (m_inds[i]->trial().dominate(*m_inds[idx])) {
				m_inds[idx]->solut() = m_inds[i]->trial();
			}
		}
		m_iter++;
		return tag;
	}

	CRDE::CRDE(param_map &v) : algorithm(v.at("algName")), m_pop(v.at("popSize")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void CRDE::initialize()	{
		m_pop.set_parameter(0.9, 0.5);
		m_pop.set_mutation_strategy(DE::mutation_strategy::rand_1);
		m_pop.initialize();
		m_pop.evaluate();
	}

	void CRDE::run_() {
		while (!terminating()) {
			m_pop.evolve();
		}
	}

	void CRDE::record() {
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
}

