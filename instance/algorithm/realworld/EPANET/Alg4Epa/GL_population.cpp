#include "GL_population.h"

namespace OFEC {

	GL_population::GL_population(size_t size_pop) : population(size_pop), m_probability(CAST_RP_EPANET->number_node()), \
		m_node_data_obj(CAST_RP_EPANET->number_node(), std::make_pair(0.0, 0))
	{

	}

	void GL_population::mutate() {
		for (size_t i = 0; i < size(); ++i) {
			m_inds[i]->mutate_first_part(m_probability);
		}
	}

	void GL_population::update_probability() {
		/// update probability of node
		
		for (auto &i : m_inds) {
			if (CAST_RP_EPANET->m_index_flag[i->variable().index() - 1]) {
				m_node_data_obj[i->variable().index() - 1].first += i->objective()[0];
				++(m_node_data_obj[i->variable().index() - 1].second);
			}
		}
		
		std::vector<real> mean_node(m_node_data_obj.size(), 0.0);
		size_t count_node = 0;
		for (size_t i = 0; i < CAST_RP_EPANET->number_node(); ++i) {
			if (m_node_data_obj[i].second != 0) {
				mean_node[i] = m_node_data_obj[i].first / m_node_data_obj[i].second;
				++count_node;
			}
		}
	
		/*real sum = 0.;
		for (auto &i : mean_node)
			sum += i;
		for (size_t i = 0; i < CAST_RP_EPANET->number_node(); ++i) {
			if (count_node > 1 && m_node_data_obj[i].second != 0)
				m_probability[i] = (1 - mean_node[i] / sum) / (count_node - 1);
			else if (count_node == 1 && m_node_data_obj[i].second != 0)
				m_probability[i] = 1;
			else m_probability[i] = 0.001;
		}*/

		real max = 0;
		for (auto &i : mean_node)
			if (max < i) max = i;
		std::vector<real> inverse;
		int size_node = mean_node.size();
		for (auto &i : mean_node) {
			if (i == 0 || i == max)
				inverse.push_back(max / size_node);
			else
				inverse.push_back(max - i);
		}
		real sum = 0;
		for (auto &i : inverse)
			sum += i;
		m_probability.clear();
		for (auto &i : inverse)
			m_probability.push_back(i / sum);
	}

	void GL_population::fill_solution(const epanet_individual & indi) {
		for (auto &i : m_inds)
			i->cover_second_part(indi);
	}

	evaluation_tag GL_population::select(bool is_stable) {
		evaluation_tag tag = evaluation_tag::Normal;
		for (auto &i : m_inds)
			tag = i->select(is_stable);

		return tag;
	}

	bool GL_population::is_feasible_population(const real tar) {

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