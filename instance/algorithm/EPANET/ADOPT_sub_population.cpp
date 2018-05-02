#include "ADOPT_sub_population.h"

namespace OFEC {
	bool ADOPT_sub_population::is_feasible_population(const float tar) {
		size_t phase = CAST_RP_EPANET->phase();
		size_t interval = CAST_RP_EPANET->interval();
		size_t num = phase*interval;
		float temp = 0;
		for (size_t i = 0; i < num; ++i) {
			for (int j = 0; j < CAST_RP_EPANET->num_sensor(); ++j) {
					temp += pow(CAST_RP_EPANET->observation_concentration()[j][i], 2);
			}
		}
		float benchmark = tar * sqrt(temp / (CAST_RP_EPANET->num_sensor()*num));

		size_t count_feasible = 0, count_infeasible = 0;
		for (size_t i = 0; i < m_pop.size(); ++i) {
			if (m_pop[i]->get_objective()[0] <= benchmark) ++count_feasible;
			else ++count_infeasible;
		}
		m_is_feasible = count_feasible >= count_infeasible;
		return m_is_feasible;
	}
	void ADOPT_sub_population::mutate() {
		for (size_t i = 0; i < m_pop.size(); ++i)
			mutate_(i, m_probability);
	}
	void ADOPT_sub_population::recombine() {
		for (auto &i : m_pop)
			i->recombine(m_CR);
	}
	evaluation_tag ADOPT_sub_population::select(bool flag) {
		evaluation_tag tag = evaluation_tag::Normal;
		for (auto &i : m_pop)
			tag = i->select(flag);
		return tag;
	}
	void ADOPT_sub_population::update_probability() {
		/// update probability of node
		for (auto &i : m_pop) {
			m_node_data_obj[i->get_variable().index() - 1].first += i->get_objective()[0];
			++(m_node_data_obj[i->get_variable().index() - 1].second);
		}
		std::vector<double> mean_node(m_node_data_obj.size(), 0.0);
		size_t count_node = 0;
		for (size_t i = 0; i < CAST_RP_EPANET->number_node(); ++i) {
			if (m_node_data_obj[i].second != 0) {
				mean_node[i] = m_node_data_obj[i].first / m_node_data_obj[i].second;
				++count_node;
			}
		}
		double sum = 0.;
		for (auto &i : mean_node)
			sum += i;
		for (size_t i = 0; i < CAST_RP_EPANET->number_node(); ++i) {
			if (m_node_data_obj[i].second != 0)
				m_probability[0][i] = (1 - mean_node[i] / sum)/(count_node -1);
			else m_probability[0][i] = 0.001;
		}

		/// update probability of starting time
		for (auto &i : m_pop) {
			m_ST_data_obj[i->get_variable().start_time() / CAST_RP_EPANET->pattern_step()].first += i->get_objective()[0];
			++(m_ST_data_obj[i->get_variable().start_time() / CAST_RP_EPANET->pattern_step()].second);
		}
		std::vector<double> mean_ST(m_ST_data_obj.size(), 0.0);
		size_t count_ST = 0;
		for (size_t i = 0; i < CAST_RP_EPANET->max_start_time_size(); ++i) {
			if (m_ST_data_obj[i].second != 0) {
				mean_ST[i] = m_ST_data_obj[i].first / m_ST_data_obj[i].second;
				++count_ST;
			}
		}
		sum = 0.;
		for (auto &i : mean_ST)
			sum += i;
		for (size_t i = 0; i < CAST_RP_EPANET->max_start_time_size(); ++i) {
			if (m_ST_data_obj[i].second != 0)
				m_probability[1][i] = (1 - mean_ST[i] / sum) / (count_ST - 1);
			else m_probability[1][i] = 0.01;
		}

		/// update probability of duration
		for (auto &i : m_pop) {
			m_duration_data_obj[i->get_variable().duration() / CAST_RP_EPANET->pattern_step()].first += i->get_objective()[0];
			++(m_duration_data_obj[i->get_variable().duration() / CAST_RP_EPANET->pattern_step()].second);
		}
		std::vector<double> mean_duration(m_duration_data_obj.size(), 0.0);
		size_t count_duration = 0;
		for (size_t i = 0; i < CAST_RP_EPANET->max_duration_size(); ++i) {
			if (m_duration_data_obj[i].second != 0) {
				mean_duration[i] = m_duration_data_obj[i].first / m_duration_data_obj[i].second;
				++count_duration;
			}
		}
		sum = 0.;
		for (auto &i : mean_duration)
			sum += i;
		for (size_t i = 0; i < CAST_RP_EPANET->max_duration_size(); ++i) {
			if (m_duration_data_obj[i].second != 0)
				m_probability[2][i] = (1 - mean_duration[i] / sum) / (count_duration - 1);
			else m_probability[2][i] = 0.01;
		}
	}
	void ADOPT_sub_population::mutate_(const int idx, const std::vector<std::vector<double>> & pro) {
		std::vector<int> ridx;
		switch (m_mutation_strategy) {
		case DE_rand_1:
			population::select(idx, 3, ridx);
			this->m_pop[idx]->mutate__(m_F, pro, m_pop[ridx[0]].get(), m_pop[ridx[1]].get(), m_pop[ridx[2]].get());
			break;
		case DE_best_1:
			population::select(idx, 2, ridx);
			this->m_pop[idx]->mutate__(m_F, pro, m_arc[0].get(), m_pop[ridx[0]].get(), m_pop[ridx[1]].get());
			break;
		case DE_target_to_best_1:
			population::select(idx, 2, ridx);
			this->m_pop[idx]->mutate__(m_F, pro, m_pop[idx].get(), m_arc[0].get(), m_pop[idx].get(), m_pop[ridx[0]].get(), m_pop[ridx[1]].get());
			break;
		case DE_best_2:
			population::select(idx, 4, ridx);
			this->m_pop[idx]->mutate__(m_F, pro, m_arc[0].get(), m_pop[ridx[0]].get(), m_pop[ridx[1]].get(), m_pop[ridx[2]].get(), m_pop[ridx[3]].get());
			break;
		case DE_rand_2:
			population::select(idx, 5, ridx);
			this->m_pop[idx]->mutate__(m_F, pro, m_pop[ridx[0]].get(), m_pop[ridx[1]].get(), m_pop[ridx[2]].get(), m_pop[ridx[3]].get(), m_pop[ridx[4]].get());
			break;
		case DE_rand_to_best_1:
			population::select(idx, 3, ridx);
			this->m_pop[idx]->mutate__(m_F, pro, m_pop[ridx[0]].get(), m_arc[0].get(), m_pop[ridx[0]].get(), m_pop[ridx[1]].get(), m_pop[ridx[2]].get());
			break;
		case DE_target_to_rand_1:
			population::select(idx, 3, ridx);
			this->m_pop[idx]->mutate__(m_F, pro, m_pop[idx].get(), m_pop[ridx[0]].get(), m_pop[idx].get(), m_pop[ridx[1]].get(), m_pop[ridx[2]].get());
			break;
		}
	}
}