#include "simulated_annealing.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../core/measure/measure.h"

namespace OFEC {
	simulated_annealing::simulated_annealing(param_map& v) : simulated_annealing(v.at("maxEvals")){}

	simulated_annealing::simulated_annealing(size_t k_max) : algorithm("SA"), m_k_max(k_max) {
		size_t size_var = CONTINUOUS_CAST->variable_size();
		size_t size_obj = CONTINUOUS_CAST->objective_size();
		size_t num_con = CONTINUOUS_CAST->num_constraints();
		m_s.reset(new solution<>(size_obj, num_con, size_var));
		m_s_new.reset(new solution<>(size_obj, num_con, size_var));
	}

	void simulated_annealing::initialize() {
		m_s->initialize();
		m_s->evaluate();
	}

	void simulated_annealing::run_() {
		while (!terminating()) {
			size_t k = global::ms_global->m_problem->evaluations();
			real T = temperature(static_cast<real>(k) / m_k_max);
			neighbour(*m_s, *m_s_new);
			m_s_new->evaluate();
			if (P(*m_s, *m_s_new, T) >= global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(0., 1.))
				*m_s = *m_s_new;
		}
	}

	void simulated_annealing::record() {
		// ******* Global Optimization ************
		size_t evals = CONTINUOUS_CAST->evaluations();
		real err = std::fabs(problem::get_sofar_best<solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
		measure::get_measure()->record(global::ms_global.get(), evals, err);
	}

	real simulated_annealing::temperature(real val)	{
		return -log10(val);
	}

	void simulated_annealing::neighbour(const solution<>& s, solution<>& s_new) {
		for (size_t i = 0; i < s.variable().size(); i++)	{
			if (CONTINUOUS_CAST->range()[i].limited) {
				real variance = (CONTINUOUS_CAST->range(i).second - CONTINUOUS_CAST->range(i).first) / 100;
				do {
					s_new.variable()[i] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(s.variable()[i], variance);
				} while (s_new.variable()[i] < CONTINUOUS_CAST->range(i).first || s.variable()[i] > CONTINUOUS_CAST->range(i).second);
			}
			else
				s_new.variable()[i] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(s.variable()[i], 100);
		}
	}

	real simulated_annealing::P(const solution<>& s, const solution<>& s_new, real T)	{
		if (s_new.dominate(s))
			return 1.;
		else
			return exp(-abs(s.objective()[0] - s_new.objective()[0]) / T);
	}
}