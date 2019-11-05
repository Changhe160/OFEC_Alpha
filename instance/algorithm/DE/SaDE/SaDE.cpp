#include "SaDE.h"
#include <algorithm>

namespace OFEC {
	SaDE_pop::SaDE_pop(size_t size_pop) : DE::population<DE::individual>(size_pop), mv_F(size_pop), \
		mvv_CR(size_pop, std::vector<real>(m_num_strategy)), m_CRm(m_num_strategy, 0.5), m_probability(m_num_strategy, 1. / m_num_strategy), \
		m_strategy_selection(size_pop) {
		for (size_t i = 0; i < m_probability.size(); ++i) {
			if (i > 0) m_probability[i] += m_probability[i - 1];
		}
	}

	evaluation_tag SaDE_pop::evolve() {
		update_CR();
		update_F();
		evaluation_tag tag = evaluation_tag::Normal;
		//std::vector<std::pair<char, real>> F;
		//int bidx = m_best[0];
		if (m_iter == 0) {
			update_best();
			//update_archive(*m_best[0]);
		}
		for (size_t i = 0; i < size(); ++i) {
			real p = global::ms_global->m_uniform[caller::Algorithm]->next()*m_probability[m_num_strategy - 1];
			m_strategy_selection[i] = lower_bound(m_probability.begin(), m_probability.end(), p) - m_probability.begin();
			//std::vector<std::pair<solution<> *, solution<> *>> dif;
			mutate(m_strategy_selection[i]);
			m_inds[i]->recombine(mvv_CR[i][m_strategy_selection[i]], m_recombine_strategy);
			if (m_inds[i]->trial().check_boundary_violation() != violation_type::None)
				m_inds[i]->trial().initialize();

		}
		for (size_t i = 0; i < size(); ++i) {
			tag = m_inds[i]->select();
			//this->update_archive(*m_pop[i]);
			if (tag != evaluation_tag::Normal) break;
		}
		if (tag == evaluation_tag::Normal) {
			m_iter++;
		}
		update_memory();
		return tag;
	}

	void SaDE_pop::update_F() {
		for (size_t i = 0; i < size(); i++) {
			mv_F[i] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(0.5, 0.09);
		}
	}

	void SaDE_pop::update_CR() {
		if (m_iter >= m_LP) {
			for (size_t k = 0; k < m_num_strategy; ++k) {
				std::vector<real> t;
				for (auto it = m_CRsuc.begin(); it != m_CRsuc.end(); ++it) {
					for (auto i : it->at(k)) t.push_back(i);
				}
				if (!t.empty()) {
					nth_element(t.begin(), t.begin() + t.size() / 2, t.end());
					m_CRm[k] = t[t.size() / 2];
				}
			}
		}
		for (size_t i = 0; i < size(); i++) {
			for (size_t k = 0; k < m_num_strategy; ++k) {
				do {
					mvv_CR[i][k] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(m_CRm[k], 0.01);
				} while (mvv_CR[i][k] < 0 || mvv_CR[i][k]>1);
			}
		}
	}

	void SaDE_pop::update_memory() {
		std::vector < std::list<real>> curmem(m_num_strategy);
		std::vector<int> curSuc(m_num_strategy), curFail(m_num_strategy);
		for (size_t i = 0; i < size(); i++) {
			if (m_inds[i]->is_improved()) {
				curmem[m_strategy_selection[i]].push_back(mvv_CR[i][m_strategy_selection[i]]);
				curSuc[m_strategy_selection[i]]++;
			}
			else {
				curFail[m_strategy_selection[i]]++;
			}
		}
		m_cnt_success.push_back(move(curSuc));
		m_CRsuc.push_back(move(curmem));
		m_cnt_fail.push_back(move(curFail));
		if (m_iter >= m_LP) {
			m_cnt_success.pop_front();
			m_CRsuc.pop_front();
			m_cnt_fail.pop_front();
			//update probability for all stategies
			for (size_t k = 0; k < m_num_strategy; ++k) {
				m_probability[k] = 0;
				int fail = 0;
				for (auto &j : m_cnt_success) m_probability[k] += j[k];
				for (auto &j : m_cnt_fail) fail += j[k];
				m_probability[k] = m_probability[k] / (m_probability[k] + fail) + m_epsilon;
				if (k > 0) m_probability[k] += m_probability[k - 1];
			}
		}
	}

	SaDE::SaDE(param_map& v) : algorithm(v.at("algName")), m_pop(v.at("popSize")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void SaDE::initialize() {
		m_pop.initialize();
		m_pop.evaluate();
	}

	void SaDE::run_() {
		while (!terminating()) {
			m_pop.evolve();

		}
	}

	void SaDE::record() {
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
