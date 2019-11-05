#include "ESs_individual.h"

namespace OFEC {
	void ESs_individual::initialize(int id) {
		individual::initialize(id);

		m_delta_mass.resize(m_var.multiplier().size(), 5.0);
		m_delta_node = 5.0;
		m_delta_start_time = 5.0;
		m_delta_add = 5.0;
		m_delta_deletion = 5.0;
		m_delta_max_depth = 5.0;
		
		m_rate_add = 0.5;
		m_rate_deletion = 0.5;
		m_max_depth = 8;
	}

	void ESs_individual::mutate(real delta_user, const std::vector<real> & pro) {

		// mutate location
		std::vector<real> roulette_node(pro.size() + 1, 0.);    // roulette wheel selection for node location
		for (size_t i = 0; i < pro.size(); ++i) {
			roulette_node[i + 1] = roulette_node[i] + pro[i];
		}
		real p = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<real>(0, roulette_node[roulette_node.size() - 1]);
		for (size_t i = 0; i < roulette_node.size() - 1; ++i)
			if (p >= roulette_node[i] && p < roulette_node[i + 1])
				m_var.index() = i + 1;

		// mutate start time
		int step = CAST_RP_EPANET->pattern_step();
		int min = CAST_RP_EPANET->m_min_start_time / step;
		int max = CAST_RP_EPANET->m_max_start_time / step;
		real now = m_var.start_time() / step;

		do {
			gauss_random(m_delta_start_time, delta_user);
		} while (m_delta_start_time <= 0);

		gauss_random(now, m_delta_start_time);
		while (!(now > min && now < max)){
			now = (min + max) / 2.0;
			gauss_random(now, m_delta_start_time);
		} 
		int now_int = now;
		real gap = now - now_int;
		if(gap > 0.5)
			m_var.start_time() = (now_int + 1) * step;
		else 
			m_var.start_time() = now_int * step;

		// mutate operation type (add, deletion, max depth)
		/*gauss_random(m_delta_add, delta_user);
		gauss_random(m_rate_add, m_delta_add);

		gauss_random(m_delta_deletion, delta_user);
		gauss_random(m_rate_deletion, m_delta_deletion);

		gauss_random(m_delta_max_depth, delta_user);
		gauss_random(m_max_depth, m_delta_max_depth);

		real sum_rate = m_rate_add + m_rate_deletion;
		real pro_add = m_rate_add / sum_rate;

		real random = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(0, 1);
		if (random < pro_add)
			mutate_add(m_max_depth);
		else
			mutate_detetion(m_max_depth);*/

		// mutate mass
		for (size_t i = 0; i < m_delta_mass.size(); ++i) {
			do {
				gauss_random(m_delta_mass[i], delta_user);
			} while (m_delta_mass[i] <= 0);
			
			gauss_random(m_var.multiplier()[i], m_delta_mass[i]);
			while(!(m_var.multiplier()[i] > CAST_RP_EPANET->m_min_multiplier && m_var.multiplier()[i] < CAST_RP_EPANET->m_max_multiplier)) {
				m_var.multiplier()[i] = (CAST_RP_EPANET->m_min_multiplier + CAST_RP_EPANET->m_max_multiplier) / 2.0;
				gauss_random(m_var.multiplier()[i], m_delta_mass[i]);
			}
		}
	}
}