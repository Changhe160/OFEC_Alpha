#include "jDE.h"


namespace OFEC {
	jDE_pop::jDE_pop(size_t size_pop) : DE::population<DE::individual>(size_pop), mv_F(size_pop), mv_CR(size_pop) {
		m_t1 = 0.1;
		m_t2 = 0.1;
		m_Fl = 0.1;
		m_Fu = 0.9;
	}

	evaluation_tag jDE_pop::evolve() {
		update_best();
		update_F_and_CR();
		evaluation_tag tag = evaluation_tag::Normal;
		std::vector<size_t> candidate(3);
		for (size_t i = 0; i < size(); ++i) {
			select(i, 3, candidate);
			m_inds[i]->mutate(mv_F[i], m_inds[candidate[0]].get(), m_inds[candidate[1]].get(), m_inds[candidate[2]].get());
			m_inds[i]->recombine(mv_CR[i], m_recombine_strategy);
			tag = m_inds[i]->select();
			if (tag != evaluation_tag::Normal) break;
		}
		if (tag == evaluation_tag::Normal) {
			++m_iter;
		}
		return tag;
	}

	void jDE_pop::update_F_and_CR() {
		std::vector<real> rand(4);
		if (m_iter > 0) {
			for (size_t i = 0; i < size(); ++i) {
				for (size_t j = 0; j < 4; ++j)
					rand[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<real>(0, 1);

				if (rand[1] < m_t1) mv_F[i] = m_Fl + rand[0] * m_Fu;
				if (rand[3] < m_t2) mv_CR[i] = rand[2];
			}
		}
		else {
			for (size_t i = 0; i < size(); ++i) {
				for (size_t j = 0; j < 4; j++)
					rand[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<real>(0, 1);

				mv_F[i] = m_Fl + rand[0] * m_Fu;
				mv_CR[i] = rand[2];
			}
		}
	}

	jDE::jDE(param_map& v) : algorithm(v.at("algName")), m_pop(v.at("popSize")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void jDE::initialize() {
		m_pop.initialize();
		m_pop.evaluate();
	}

	void jDE::run_(){
		while (!terminating()) {
			m_pop.evolve();
		}
	}

	void jDE::record() {
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


