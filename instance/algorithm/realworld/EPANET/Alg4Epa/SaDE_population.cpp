#include "SaDE_population.h"
#include "../../../../problem/realworld/EPANET/epanet.h"

namespace OFEC {
	SaDE_population::SaDE_population(size_t size_pop) : population(size_pop), m_F(size_pop), \
		mvv_CR(size_pop, std::vector<real>(m_num_strategy)), m_CRm(m_num_strategy, 0.5), m_pro_strategy(m_num_strategy, 1. / m_num_strategy), \
		m_strategy_selection(size_pop), m_probability(2), \
		m_ST_data_obj(CAST_RP_EPANET->max_start_time_size(), std::make_pair(0.0, 0)), \
		m_duration_data_obj(CAST_RP_EPANET->max_duration_size(), std::make_pair(0.0, 0)) \
	{
		m_probability[0].resize(CAST_RP_EPANET->max_start_time_size());
		m_probability[1].resize(CAST_RP_EPANET->max_duration_size());

		for (auto i = 0; i < m_pro_strategy.size(); ++i) {
			if (i > 0) m_pro_strategy[i] += m_pro_strategy[i - 1];
		}
	}

	void SaDE_population::update_F() {
		for (auto i = 0; i < size(); i++) {
			m_F[i] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(0.5, 0.09);
		}
	}

	void SaDE_population::update_CR() {
		if (m_iter >= m_LP) {
			for (int k = 0; k < m_num_strategy; ++k) {
				std::vector<real> t;
				for (auto it = m_CRsuc.begin(); it != m_CRsuc.end(); ++it) {
					for (auto i : it->at(k)) t.push_back(i);
				}
				if (t.size() > 0) {
					nth_element(t.begin(), t.begin() + t.size() / 2, t.end());
					m_CRm[k] = t[t.size() / 2];
				}
			}
		}
		for (auto i = 0; i < size(); i++) {
			for (int k = 0; k < m_num_strategy; ++k) {
				do {
					mvv_CR[i][k] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(m_CRm[k], 0.01);
				} while (mvv_CR[i][k] < 0 || mvv_CR[i][k]>1);
			}
		}

	}

	void SaDE_population::update_pro_strategy() {

		std::vector < std::list<real>> curmem(m_num_strategy);
		std::vector<int> curSuc(m_num_strategy), curFail(m_num_strategy);
		for (auto i = 0; i < size(); i++) {
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
			real sum = 0;
			for (int k = 0; k < m_num_strategy; ++k) {
				m_pro_strategy[k] = 0;
				int fail = 0;
				for (auto &j : m_cnt_success) m_pro_strategy[k] += j[k];
				for (auto &j : m_cnt_fail) fail += j[k];

				m_pro_strategy[k] = m_pro_strategy[k] / (m_pro_strategy[k] + fail) + m_epsilon;
				if (k > 0) m_pro_strategy[k] += m_pro_strategy[k - 1];
			}
		}
	}

	void SaDE_population::mutate() {
		for (size_t i = 0; i < size(); ++i) {
			real p = global::ms_global->m_uniform[caller::Algorithm]->next()*m_pro_strategy[m_num_strategy - 1];
			m_strategy_selection[i] = lower_bound(m_pro_strategy.begin(), m_pro_strategy.end(), p) - m_pro_strategy.begin();
			set_mutation_strategy(DE_mutation_stratgy(m_strategy_selection[i]));
			update_best();
			mutate_(i, m_F[i], m_probability);
		}
	}

	void SaDE_population::set_mutation_strategy(DE_mutation_stratgy rS) {
		m_mutation_strategy = rS;
	}

	void SaDE_population::recombine() {
		for (size_t i = 0; i < size(); ++i)
			m_inds[i]->recombine(mvv_CR[i][m_strategy_selection[i]]);
	}
	
	void SaDE_population::update_probability() {
		
		/// update probability of starting time
		for (auto &i : m_inds) {
			int idx = (i->variable().start_time() - CAST_RP_EPANET->m_min_start_time) / CAST_RP_EPANET->pattern_step();
			m_ST_data_obj[idx].first += i->objective()[0];
			++(m_ST_data_obj[idx].second);
		}
		std::vector<real> mean_ST(m_ST_data_obj.size(), 0.0);
		size_t count_ST = 0;
		for (size_t i = 0; i < CAST_RP_EPANET->max_start_time_size(); ++i) {
			if (m_ST_data_obj[i].second != 0) {
				mean_ST[i] = m_ST_data_obj[i].first / m_ST_data_obj[i].second;
				++count_ST;
			}
		}
		/*real sum = 0.;
		for (auto &i : mean_ST)
			sum += i;
		for (size_t i = 0; i < CAST_RP_EPANET->max_start_time_size(); ++i) {
			if (count_ST > 1 && m_ST_data_obj[i].second != 0)
				m_probability[0][i] = (1 - mean_ST[i] / sum) / (count_ST - 1);
			else if (count_ST == 1 && m_ST_data_obj[i].second != 0)
				m_probability[0][i] = 1;
			else m_probability[0][i] = 0.01;
		}*/

		real max = 0;
		for (auto &i : mean_ST)
			if (max < i) max = i;
		std::vector<real> inverse;
		int size_ST = mean_ST.size();
		for (auto &i : mean_ST) {
			if (i == 0 || i == max)
				inverse.push_back(max / size_ST);
			else
				inverse.push_back(max - i);
		}
		real sum = 0;
		for (auto &i : inverse)
			sum += i;
		m_probability[0].clear();
		for (auto &i : inverse)
			m_probability[0].push_back(i / sum);

		/// update probability of duration
		for (auto &i : m_inds) {
			int idx = (i->variable().duration() - CAST_RP_EPANET->m_min_duration) / CAST_RP_EPANET->pattern_step();
			m_duration_data_obj[idx].first += i->objective()[0];
			++(m_duration_data_obj[idx].second);
		}
		std::vector<real> mean_duration(m_duration_data_obj.size(), 0.0);
		size_t count_duration = 0;
		for (size_t i = 0; i < CAST_RP_EPANET->max_duration_size(); ++i) {
			if (m_duration_data_obj[i].second != 0) {
				mean_duration[i] = m_duration_data_obj[i].first / m_duration_data_obj[i].second;
				++count_duration;
			}
		}
		/*sum = 0.;
		for (auto &i : mean_duration)
			sum += i;
		for (size_t i = 0; i < CAST_RP_EPANET->max_duration_size(); ++i) {
			if (count_duration > 1 && m_duration_data_obj[i].second != 0)
				m_probability[1][i] = (1 - mean_duration[i] / sum) / (count_duration - 1);
			else if (count_duration == 1 && m_duration_data_obj[i].second != 0)
				m_probability[1][i] = 1;
			else m_probability[1][i] = 0.01;
		}*/

		max = 0;
		for (auto &i : mean_duration)
			if (max < i) max = i;
		inverse.clear();
		int size_duration = mean_duration.size();
		for (auto &i : mean_duration) {
			if (i == 0 || i == max)
				inverse.push_back(max / size_duration);
			else
				inverse.push_back(max - i);
		}
		sum = 0;
		for (auto &i : inverse)
			sum += i;
		m_probability[1].clear();
		for (auto &i : inverse)
			m_probability[1].push_back(i / sum);
	}

	void SaDE_population::mutate_(const int idx, real F, const std::vector<std::vector<real>> & pro) {
		std::vector<int> ridx;
		switch (m_mutation_strategy) {
		case rand_1:
			select(idx, 3, ridx);
			this->m_inds[idx]->mutate_second_part(F, pro, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
			break;
		case best_1:
			select(idx, 2, ridx);
			this->m_inds[idx]->mutate_second_part(F, pro, m_best[0].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get());
			break;
		case target_to_best_1:
			select(idx, 2, ridx);
			this->m_inds[idx]->mutate_second_part(F, pro, m_inds[idx].get(), m_best[0].get(), m_inds[idx].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get());
			break;
		case best_2:
			select(idx, 4, ridx);
			this->m_inds[idx]->mutate_second_part(F, pro, m_best[0].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), m_inds[ridx[3]].get());
			break;
		case rand_2:
			select(idx, 5, ridx);
			this->m_inds[idx]->mutate_second_part(F, pro, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), m_inds[ridx[3]].get(), m_inds[ridx[4]].get());
			break;
		case rand_to_best_1:
			select(idx, 3, ridx);
			this->m_inds[idx]->mutate_second_part(F, pro, m_inds[ridx[0]].get(), m_best[0].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
			break;
		case target_to_rand_1:
			select(idx, 3, ridx);
			this->m_inds[idx]->mutate_second_part(F, pro, m_inds[idx].get(), m_inds[ridx[0]].get(), m_inds[idx].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
			break;
		}
	}

	void SaDE_population::select(int base, int number, std::vector<int>& result) {
		std::vector<int> candidate;
		for (int i = 0; i < this->m_inds.size(); ++i) {
			if (base != i) candidate.push_back(i);
		}
		result.resize(number);
		for (int i = 0; i < number; ++i) {
			int idx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)candidate.size() - i);
			result[i] = candidate[idx];
			if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
		}
	}

	void SaDE_population::fill_solution(const epanet_individual & indi) {
		for (auto &i : m_inds)
			i->cover_first_part(indi);
	}

	evaluation_tag SaDE_population::select(bool is_stable) {
		evaluation_tag tag = evaluation_tag::Normal;
		for (auto &i : m_inds)
			tag = i->select(is_stable);

		return tag;
	}

	bool SaDE_population::is_feasible_population(const real tar) {

		size_t phase = CAST_RP_EPANET->phase();
		size_t interval = CAST_RP_EPANET->interval();
		size_t num = phase*interval;
		real temp = 0;

		for (size_t i = 0; i < num; ++i) {
			for (int j = 0; j < CAST_RP_EPANET->num_sensor(); ++j) {
				temp += pow(CAST_RP_EPANET->observation_concentration()[j][i], 2);
			}
		}

		real benchmark = tar * sqrt(temp / (CAST_RP_EPANET->num_sensor()*num));

		size_t count_feasible = 0, count_infeasible = 0;
		for (size_t i = 0; i < m_inds.size(); ++i) {
			if (m_inds[i]->objective()[0] <= benchmark) ++count_feasible;
			else ++count_infeasible;
		}

		return count_feasible >= count_infeasible;
	}
}