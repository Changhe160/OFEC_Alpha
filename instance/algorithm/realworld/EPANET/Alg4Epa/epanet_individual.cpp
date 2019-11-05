#include "epanet_individual.h"


namespace OFEC {
	void epanet_individual::mutate_first_part(const std::vector<real> & pro) {
		m_pu.variable() = m_var;
		m_pu.variable().is_detected() = false;

		mutate_location(pro);
	}

	void epanet_individual::mutate_second_part(real F, const std::vector<std::vector<real>> & pro,
		solution<variable_epanet, real> *r1,
		solution<variable_epanet, real> *r2,
		solution<variable_epanet, real> *r3,
		solution<variable_epanet, real> *r4,
		solution<variable_epanet, real> *r5) {
		m_pv.variable() = m_var;
		m_pv.variable().is_detected() = false;

		//mutate_ST(pro[0]);
		//mutate_duration(pro[1]);
		mutate_mass(F, r1, r2, r3, r4, r5);
	}

	void epanet_individual::mutate_mass(real F, solution<variable_epanet, real> *r1,
		solution<variable_epanet, real> *r2,
		solution<variable_epanet, real> *r3,
		solution<variable_epanet, real> *r4,
		solution<variable_epanet, real> *r5) {

		/// mutate multiplier
		long u_multi = CAST_RP_EPANET->m_max_multiplier;
		long l_multi = CAST_RP_EPANET->m_min_multiplier;
		long u_st = CAST_RP_EPANET->m_max_start_time;
		long l_st = CAST_RP_EPANET->m_min_start_time;

		size_t pv_size = m_pv.variable().duration() / CAST_RP_EPANET->pattern_step();
		if (m_pv.variable().duration() % CAST_RP_EPANET->pattern_step() != 0)
			++pv_size;
		++pv_size;
		long time_step = CAST_RP_EPANET->pattern_step();
		std::vector<float> vec_r1(r1->variable().multiplier());
		vec_r1.push_back(r1->variable().start_time() / time_step);
		std::vector<float> vec_r2(r2->variable().multiplier());
		vec_r2.push_back(r2->variable().start_time() / time_step);
		std::vector<float> vec_r3(r3->variable().multiplier());
		vec_r3.push_back(r3->variable().start_time() / time_step);
		vec_r1.resize(pv_size);
		vec_r2.resize(pv_size);
		vec_r3.resize(pv_size);
		/*for (auto&i : vec_r1)
		if (i == 0)
		i = l_multi;
		for (auto&i : vec_r2)
		if (i == 0)
		i = l_multi;
		for (auto&i : vec_r3)
		if (i == 0)
		i = l_multi;*/

		std::vector<float> vec_r4;
		std::vector<float> vec_r5;
		if (r4&&r5) {
			vec_r4 = r4->variable().multiplier();
			vec_r4.push_back(r4->variable().start_time() / time_step);
			vec_r5 = r5->variable().multiplier();
			vec_r5.push_back(r5->variable().start_time() / time_step);
			vec_r4.resize(pv_size);
			vec_r5.resize(pv_size);
			/*for (auto&i : vec_r4)
			if (i == 0)
			i = l_multi;
			for (auto&i : vec_r5)
			if (i == 0)
			i = l_multi;*/
		}
		float temp;
		for (size_t i = 0; i < pv_size; ++i) {
			if (i == pv_size - 1) {
				temp = vec_r1[i] + F*(vec_r2[i] - vec_r3[i]);
				if (r4&&r5) temp += F*(vec_r4[i] - vec_r5[i]);

				if (temp >(u_st / time_step)) {
					temp = (vec_r1[i] + u_st) / 2.;
				}
				else if (temp < (l_st / time_step)) {
					temp = ((vec_r1[i]) + l_st) / 2.;
				}
				if (temp - (int)temp >= (int)temp + 1 - temp)
					m_pv.variable().start_time() = ((int)temp + 1) * time_step;
				else
					m_pv.variable().start_time() = (int)temp * time_step;
			}
			else {
				m_pv.variable().multiplier()[i] = (vec_r1[i]) + F*((vec_r2[i]) - (vec_r3[i]));
				if (r4&&r5) m_pv.variable().multiplier()[i] += F*((vec_r4[i]) - (vec_r5[i]));

				if ((m_pv.variable().multiplier()[i]) > u_multi) {
					m_pv.variable().multiplier()[i] = ((vec_r1[i]) + u_multi) / 2;
				}
				else if ((m_pv.variable().multiplier()[i]) < l_multi) {
					m_pv.variable().multiplier()[i] = ((vec_r1[i]) + l_multi) / 2;
				}
			}
		}

	}

	void epanet_individual::mutate_location(const std::vector<real> & pro) {

		std::vector<real> roulette_node(pro.size() + 1, 0.);    // roulette wheel selection for node location
		for (size_t i = 0; i < pro.size(); ++i) {
			roulette_node[i + 1] = roulette_node[i] + pro[i];
		}
		real p = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<real>(0, roulette_node[roulette_node.size() - 1]);
		for (size_t i = 0; i < roulette_node.size() - 1; ++i)
			if (p >= roulette_node[i] && p < roulette_node[i + 1])
				m_pu.variable().index() = i + 1;
	}

	void epanet_individual::mutate_ST(const std::vector<real> & pro) {
		/// mutate starting time

		std::vector<real> roulette_ST(pro.size() + 1, 0.);    // roulette wheel selection for starting time
		for (size_t i = 0; i < pro.size(); ++i) {
			roulette_ST[i + 1] = roulette_ST[i] + pro[i];
		}
		real p = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<real>(0, roulette_ST[roulette_ST.size() - 1]);
		for (size_t i = 0; i < roulette_ST.size() - 1; ++i)
			if (p >= roulette_ST[i] && p < roulette_ST[i + 1])
				m_pv.variable().start_time() = i*CAST_RP_EPANET->pattern_step();


	}

	void epanet_individual::mutate_duration(const std::vector<real> & pro) {
		m_pv.variable().duration() = m_var.duration();

		/*std::vector<real> roulette_duration(pro[2].size() + 1, 0.);    // roulette wheel selection for duration
		sum = 0.;
		for (size_t i = 0; i < pro[2].size(); ++i) {
		sum += pro[2][i];
		roulette_duration[i + 1] = sum;
		}
		p = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<real>(0, roulette_duration[roulette_duration.size() - 1]);
		for (size_t i = 0; i < roulette_duration.size()-1; ++i)
		if (p > roulette_duration[i] && p < roulette_duration[i + 1])
		m_pv.variable().duration() = i*CAST_RP_EPANET->pattern_step();
		*/
	}

	void epanet_individual::recombine(real CR) {
		m_pu = m_pv;

		size_t pv_size_multiplier = m_pv.variable().multiplier().size();
		std::vector<float> mult_temp(variable().multiplier());
		mult_temp.push_back(variable().start_time());
		++pv_size_multiplier;
		mult_temp.resize(pv_size_multiplier);

		/*for (auto &i : mult_temp)
		if (i == 0)
		i = CAST_RP_EPANET->m_min_multiplier;*/
		size_t I = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<size_t>(0, pv_size_multiplier);
		for (size_t i = 0; i < pv_size_multiplier; ++i) {
			real p = global::ms_global->m_uniform[caller::Algorithm]->next();
			if (i == pv_size_multiplier - 1) {
				if (p <= CR || i == I) {
					m_pu.variable().start_time() = m_pv.variable().start_time();
				}
				else {
					m_pu.variable().start_time() = mult_temp[i];
				}
			}
			else {
				if (p <= CR || i == I) {
					m_pu.variable().multiplier()[i] = m_pv.variable().multiplier()[i];
				}
				else {
					m_pu.variable().multiplier()[i] = mult_temp[i];
				}
			}
		}

	}

	evaluation_tag epanet_individual::select(bool is_stable) {

		evaluation_tag tag = m_pu.evaluate();

		if ((is_stable && (m_pu_distance_fitness > m_distance_fitness)) || (!is_stable && m_pu.dominate(m_obj))) {
			m_var = m_pu.variable();
			m_obj = m_pu.objective();
			m_constraint_value = m_pu.constraint_value();
			m_improved = true;
		}
		else {
			m_improved = false;
		}
		return tag;
	}

	bool epanet_individual::same_location(epanet_individual & indi) {
		return m_var.index() == indi.variable().index();
	}

	void epanet_individual::initialize(int id) {
		individual::initialize(id);
		//m_flag = true;

		m_pu.variable() = variable();
		m_pu.objective() = objective();
		m_pu.constraint_value() = constraint_value();

		m_pv.variable() = variable();
		m_pv.objective() = objective();
		m_pv.constraint_value() = constraint_value();

		//m_pv = std::move(std::unique_ptr<solution_type>(new solution_type()));
	}

	solution<variable_epanet, real> & epanet_individual::trial() {
		return m_pu;
	}

	void epanet_individual::cover_first_part(const epanet_individual & indi) {
		m_var.is_detected() = false;
		std::strcpy(m_var.location(), indi.variable().location());
		m_var.index() = indi.variable().index();
	}

	void epanet_individual::cover_second_part(const epanet_individual & indi) {
		m_var.is_detected() = false;
		m_var.start_time() = indi.variable().start_time();
		m_var.duration() = indi.variable().duration();
		m_var.multiplier() = indi.variable().multiplier();
	}
}