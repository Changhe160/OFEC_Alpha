#include "NCDE.h"

namespace OFEC {
	NCDE_pop::NCDE_pop(size_t size_pop) : DE::population<DE::individual>(size_pop), m_dis(size_pop) {
		m_r = 0.1;
		m_m = static_cast<size_t>(m_r * size());
		m_F = 0.5;
		m_CR = 0.7;
		for (auto &i : m_inds) {
			i->set_improved_flag(true);
		}
	}

	void NCDE_pop::sort_distance(size_t a) {
		if (!m_inds[a]->is_improved()) {
			for (size_t j = 0; j < size(); ++j) {
				if (a == j) continue;
				if (m_inds[j]->is_improved()) {
					std::pair<real, int> dis = std::make_pair(m_inds[a]->variable_distance(*m_inds[j]), j);
					auto it = m_dis[a].begin();
					while (it != m_dis[a].end() && it->first < dis.first) {
						++it;
					}
					if (it != m_dis[a].end()) {
						m_dis[a].insert(it, dis);
						m_dis[a].pop_back();
					}
				}
			}
		}
		else {
			for (size_t j = 0; j < size(); ++j) {
				if (a == j) continue;
				std::pair<real, int> dis = std::make_pair(m_inds[a]->variable_distance(*m_inds[j]), j);
				auto it = m_dis[a].begin();
				while (it != m_dis[a].end() && it->first < dis.first) {
					it++;
				}
				if (m_dis[a].size() >= m_m) {
					if (it != m_dis[a].end()) {
						m_dis[a].insert(it, dis);
						m_dis[a].pop_back();
					}
				}
				else {
					m_dis[a].insert(it, dis);
				}
			}
		}

	}

	evaluation_tag NCDE_pop::evolve() {
		update_best();
		evaluation_tag tag = evaluation_tag::Normal;
		size_t nearest_index = 0;
		real neardis;
		for (size_t i = 0; i < size(); ++i) {
			sort_distance(i);
			std::vector<size_t> candidate;
			for (auto it = m_dis[i].begin(); it != m_dis[i].end(); it++) {
				candidate.push_back(it->second);
			}
			std::vector<size_t> result(3);
			select_in_neighborhood(3, candidate, result);
			m_inds[i]->mutate(m_F, m_inds[result[0]].get(), m_inds[result[1]].get(), m_inds[result[2]].get());
			m_inds[i]->recombine(m_CR, m_recombine_strategy);
			tag = m_inds[i]->select();
			if (m_inds[i]->is_improved()) {
				neardis = std::numeric_limits<real>::max();
				for (size_t j = 0; j < size(); ++j) {
					if (i == j) continue;
					real dis = m_inds[i]->variable_distance(*m_inds[j]);
					if (neardis > dis) {
						neardis = dis;
						nearest_index = j;
					}
				}
			}
			else {
				nearest_index = m_dis[i].begin()->second;
			}
			if (m_inds[i]->dominate(*m_inds[nearest_index])) {
				*m_inds[nearest_index] = *m_inds[i];
				m_inds[nearest_index]->set_improved_flag(true);
			}
			if (tag != evaluation_tag::Normal) break;
		}
		if (tag == evaluation_tag::Normal) {
			++m_iter;
		}
		return tag;
	}

	NCDE::NCDE(param_map& v) : algorithm(v.at("algName")), m_pop(v.at("popSize")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void NCDE::initialize() {
		m_pop.initialize();
		m_pop.evaluate();
	}

	void NCDE::run_() {
		while (!terminating()) {
			m_pop.evolve();
		}
	}

	void NCDE::record() {
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



