#include "NCDE.h"

namespace OFEC {
	NCDE::NCDE(param_map &v) :DEpopulation(v[param_popSize], v[param_numDim]), \
		m_dis(v[param_popSize])  \
	{
		m_r = 0.1;
		m_m = m_r*size();
		m_F = 0.5;
		m_CR = 0.7;
		for (auto &i : m_pop) {
			i->set_improve_flag(true);
		}
	}

	void NCDE::sort_distanece(int a) {
		if (!m_pop[a]->is_improved()) {
			for (int j = 0; j < size(); ++j) {
				if (a == j) continue;
				if (m_pop[j]->is_improved()) {
					std::pair<double, int> dis = std::make_pair(m_pop[a]->variable_distance(*m_pop[j]),j);
					auto it = m_dis[a].begin();
					while (it != m_dis[a].end() && it->first < dis.first) {
						it++;
					}
					if (it != m_dis[a].end()) {
						m_dis[a].insert(it, dis);
						m_dis[a].pop_back();
					}
				}
			}
		}
		else {
			for (int j = 0; j < size(); ++j) {
				if (a == j) continue;
				std::pair<double, int> dis = std::make_pair(m_pop[a]->variable_distance(*m_pop[j]), j);
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


	evaluation_tag NCDE::evolve() {
		evaluation_tag tag = evaluation_tag::Normal;
		int NearestIdx;
		double neardis = std::numeric_limits<double>::max();
		for (int i = 0; i < size(); ++i) {
			sort_distanece(i);
		
			std::vector<int> candidate;
			for (auto it = m_dis[i].begin(); it != m_dis[i].end(); it++) {
				candidate.push_back(it->second);
			}
			std::vector<int> result(3);
			selectInNeighb(3, candidate, result);

			m_pop[i]->mutate(m_F, m_pop[result[0]].get(), m_pop[result[1]].get(), m_pop[result[2]].get());
			m_pop[i]->recombine(m_CR);
			tag = m_pop[i]->select();
			if (m_pop[i]->is_improved()) {
				neardis = std::numeric_limits<double>::max();
				for (int j = 0; j < size(); ++j) {
					if (i == j) continue;
					double dis = m_pop[i]->variable_distance(*m_pop[j]);
					if (neardis > dis) {
						neardis = dis;
						NearestIdx = j;
					}
				}

			}
			else {
				NearestIdx = m_dis[i].begin()->second;
			}
			if (m_pop[i]->dominate(*m_pop[NearestIdx])) {
				m_pop[NearestIdx] = m_pop[i];

				m_pop[NearestIdx]->set_improve_flag(true);
			}
			if (tag != evaluation_tag::Normal) break;
		}
		if (tag == evaluation_tag::Normal) {
			++m_iter;
		}
		return tag;

	}

	evaluation_tag NCDE::run_() {
		evaluation_tag tag = evaluation_tag::Normal;
		std::vector<double> gopt(1);
		gopt = CONTINOUS_CAST->get_optima().multi_objective(0);

		while (tag != evaluation_tag::Terminate) {
			//g_mutexStream.lock();
		
			//update_best();
			double best = problem::get_sofar_best<solution<>>(0)->get_objective()[0];
			double error = fabs(best - gopt[0]);
			int num_opt_found = CONTINOUS_CAST->get_optima().num_optima_found();
			//std::cout << m_iter << " " << error <<" "<< m_best[0]->get_variable()[0] << " " << m_best[0]->get_variable()[1] << std::endl;
			std::cout << m_iter << " " << CONTINOUS_CAST->total_evaluations() << " " << num_opt_found << std::endl;
			//g_mutexStream.unlock();
			measure::ms_measure->record(global::ms_global.get(), m_iter, num_opt_found);
			tag = evolve();
		}
		
		return tag;
	}
}



