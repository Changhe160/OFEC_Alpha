
#include "SPSO11.h"
#include "../../../../core/global.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../utility/vector.h"
#include <numeric>

namespace OFEC {
	void particle11::initialize_velocity() {
		for (size_t j = 0; j < m_var.size(); j++) {
			auto& range = CONTINUOUS_CAST->range(j);
			m_vel[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(range.first - m_var[j], range.second- m_var[j]);
		}
	}

	void particle11::clamp_velocity() {
		for (size_t j = 0;  j<m_var.size(); j++) {
			auto& range = CONTINUOUS_CAST->range(j);
			if (m_var[j]>range.second) {
				m_var[j] = range.second;
				m_vel[j] *= -0.5;
			}
			else if (m_var[j]<range.first) {
				m_var[j] = range.first;
				m_vel[j] *= -0.5;
			}
		}
	}

	void particle11::next_velocity(const solution<> &lbest, real w, real c1, real c2) {

		std::vector<real> G(m_var.size());

		if (&lbest != &m_pbest) {
			for (size_t j = 0;  j < m_var.size(); j++) 
				G[j] = m_var[j] + c1*(m_pbest.variable()[j] + lbest.variable()[j] - 2 * m_var[j]) / 3;
		}
		else {
			for (size_t j = 0;  j < m_var.size(); j++)
				G[j] = m_var[j] + c1*(m_pbest.variable()[j] -  m_var[j]) / 2;
		}
		
		real dis = 0;
		for (size_t j = 0;  j < m_var.size(); j++) 
			dis += (G[j] - m_var[j])*(G[j] - m_var[j]);
		dis = sqrt(dis);

		Vector v(m_var.size(),0);
		v.randomize_in_sphere(dis,global::ms_global->m_uniform[caller::Algorithm].get());
		v += G;

		for (size_t j = 0;  j < m_var.size(); j++) 
			m_vel[j] = w*m_vel[j] + v[j] - m_var[j];
		
	}

	SPSO11::SPSO11(param_map& v) : algorithm(v.at("algName")), m_pop(v.at("popSize")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void SPSO11::initialize() {
		//m_pop.initialize_parameters(0.721, 1.193, 1.193); //0.7298f, 1.496f, 1.496f
		m_pop.weight() = 0.721;
		m_pop.accelerator1() = m_pop.accelerator2() = 1.193;
		m_pop.initialize();
	}

	void SPSO11::run_() {
		while (!terminating()) {
			m_pop.evolve();
		}
	}

	void SPSO11::record() {
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