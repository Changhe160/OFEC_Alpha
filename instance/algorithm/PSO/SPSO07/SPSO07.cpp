#include "SPSO07.h"
#include "../../../../core/global.h"
#include "../../../../core/problem/continuous/continuous.h"
#include <numeric>

namespace OFEC {

	void particle07::initialize_velocity() {
		for (size_t j = 0; j<m_var.size(); j++) {
			auto& range = CONTINUOUS_CAST->range(j);
			m_vel[j] = (global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(range.first,range.second) - m_var[j]) / 2;
		}
	}

	void particle07::next_velocity(const solution<> &lbest, real w, real c1, real c2) {
		if (&lbest != &m_pbest) {
			for (size_t j = 0; j<m_var.size(); j++) {
				m_vel[j] = w * m_vel[j] + c1 * global::ms_global->m_uniform[caller::Algorithm]->next()*(m_pbest.variable()[j] - m_var[j]) + c2 * global::ms_global->m_uniform[caller::Algorithm]->next()*(lbest.variable()[j] - m_var[j]);
			}
		}
		else {
			for (size_t j = 0; j<m_var.size(); j++) {
				m_vel[j] = w * m_vel[j] + c1 * global::ms_global->m_uniform[caller::Algorithm]->next()*(m_pbest.variable()[j] - m_var[j]);
			}
		}

	}

	SPSO07::SPSO07(param_map &v): algorithm(v.at("algName")), m_pop(v.at("popSize")){
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void SPSO07::initialize() {
		
		//m_pop.initialize_parameters(0.721, 1.193, 1.193); //0.7298f, 1.496f, 1.496f
		m_pop.weight() = 0.721;
		m_pop.accelerator1() = m_pop.accelerator2() = 1.193;
		m_pop.initialize();
	}

	void SPSO07::run_() {
		while (!terminating()) {
			m_pop.evolve();
		}
	}

	void SPSO07::record() {
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

	void SPSO07::update_parameters() {
		
		this->m_parameters["inertia weight"] = m_pop.weight();
		this->m_parameters["accelerator1"] = m_pop.accelerator1();
		this->m_parameters["accelerator2"] = m_pop.accelerator2();
	}
}