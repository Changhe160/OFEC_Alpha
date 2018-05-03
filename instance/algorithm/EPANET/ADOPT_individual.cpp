#include "ADOPT_individual.h"

namespace OFEC {
	
	void ADOPT_individual::mutate__(double F, const std::vector<std::vector<double>> & pro, solution<variable_epanet, real> *r1,
		solution<variable_epanet, real> *r2,
		solution<variable_epanet, real> *r3,
		solution<variable_epanet, real> *r4,
		solution<variable_epanet, real> *r5) {
		
		/*std::vector<solution<variable_epanet, real> *> sol;
		sol.push_back(r1);
		sol.push_back(r2);
		sol.push_back(r3);
		if (r4&&r5) {
			sol.push_back(r4);
			sol.push_back(r5);
		}
		size_t best_idx = 0;
		for (size_t i = 0; i < sol.size() - 1; ++i) {
			if (sol[i + 1]->dominate(*sol[i])) best_idx = i + 1;
		}*/

		/// mutate index of nodes
		//int u_node = CAST_RP_EPANET->number_node();
		//int l_node = 1;
		//m_pv.get_variable().index() = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(l_node, u_node +1);
		//m_pv.get_variable().index() = sol[best_idx]->get_variable().index();
		//m_pv.get_variable().index() = r1->get_variable().index();
		std::vector<double> roulette_node(pro[0].size()+1, 0.);    // roulette wheel selection for node location
		double sum = 0.;
		for (size_t i = 0; i < pro[0].size(); ++i) {
			sum += pro[0][i];
			roulette_node[i+1] = sum;
		}
		double p = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<double>(0, roulette_node[roulette_node.size() - 1]);
		for (size_t i = 0; i < roulette_node.size()-1; ++i)
			if (p > roulette_node[i] && p < roulette_node[i + 1])
				m_pv.get_variable().index() = i + 1;

		/// mutate index of starting time
		/*
		long u_start = CAST_RP_EPANET->m_max_start_time;
		long l_start = CAST_RP_EPANET->m_min_start_time;
		m_pv.get_variable().start_time() = (r1->get_variable().start_time()) + F*((r2->get_variable().start_time()) - (r3->get_variable().start_time()));
		if (r4&&r5) m_pv.get_variable().start_time() += F*((r4->get_variable().start_time()) - (r5->get_variable().start_time()));

		if ((m_pv.get_variable().start_time()) > u_start) {
		m_pv.get_variable().start_time() = ((r1->get_variable().start_time()) + u_start) / 2;
		}
		else if ((m_pv.get_variable().start_time()) < l_start) {
		m_pv.get_variable().start_time() = ((r1->get_variable().start_time()) + l_start) / 2;
		}*/
		std::vector<double> roulette_ST(pro[1].size() + 1, 0.);    // roulette wheel selection for starting time
		sum = 0.;
		for (size_t i = 0; i < pro[1].size(); ++i) {
			sum += pro[1][i];
			roulette_ST[i + 1] = sum;
		}
		p = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<double>(0, roulette_ST[roulette_ST.size() - 1]);
		for (size_t i = 0; i < roulette_ST.size()-1; ++i)
			if (p > roulette_ST[i] && p < roulette_ST[i + 1])
				m_pv.get_variable().start_time() = i*CAST_RP_EPANET->pattern_step();

		

		/// mutate duration
		std::vector<size_t> size_mass;
		size_mass.emplace_back(r1->get_variable().multiplier().size());
		size_mass.emplace_back(r2->get_variable().multiplier().size());
		size_mass.emplace_back(r3->get_variable().multiplier().size());
		if (r4&&r5) {
			size_mass.emplace_back(r4->get_variable().multiplier().size());
			size_mass.emplace_back(r5->get_variable().multiplier().size());
		}
		size_t max_size = size_mass[0];
		for (size_t i = 0; i < size_mass.size(); ++i) 
			if (i != 0 && size_mass[i] > max_size) max_size = size_mass[i];
		++max_size;

		std::vector<double> roulette_duration(max_size + 1, 0.);    // roulette wheel selection for duration
		sum = 0.;
		for (size_t i = 0; i < max_size; ++i) {
			sum += pro[2][i];
			roulette_duration[i + 1] = sum;
		}
		p = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<double>(0, roulette_duration[roulette_duration.size() - 1]);
		for (size_t i = 0; i < roulette_duration.size()-1; ++i)
			if (p > roulette_duration[i] && p < roulette_duration[i + 1])
				m_pv.get_variable().duration() = i*CAST_RP_EPANET->pattern_step();
		/*long u_dur = CAST_RP_EPANET->m_max_duration;
		long l_dur = CAST_RP_EPANET->m_min_duration;
		m_pv.get_variable().duration() = (r1->get_variable().duration()) + F*((r2->get_variable().duration()) - (r3->get_variable().duration()));
		if (r4&&r5) m_pv.get_variable().duration() += F*((r4->get_variable().duration()) - (r5->get_variable().duration()));

		if ((m_pv.get_variable().duration()) > u_dur) {
			m_pv.get_variable().duration() = ((r1->get_variable().duration()) + u_dur) / 2;
		}
		else if ((m_pv.get_variable().duration()) < l_dur) {
			m_pv.get_variable().duration() = ((r1->get_variable().duration()) + l_dur) / 2;
		}*/

		/*std::vector<std::unique_ptr<solution<variable_epanet, real>>> sol;
		sol.emplace(new solution<variable_epanet, real>(r1));
		sol.emplace(new solution<variable_epanet, real>(r2));
		sol.emplace(new solution<variable_epanet, real>(r3));
		if (r4&&r5) {
			sol.emplace(new solution<variable_epanet, real>(r4));
			sol.emplace(new solution<variable_epanet, real>(r5));
		}
		size_t best_idx = 0;
		for (size_t i = 0; i < sol.size()-1; ++i) {
			if (sol[i + 1]->dominate(sol[i])) best_idx = i + 1;
		}
		m_pv.get_variable().duration() = sol[best_idx]->get_variable().duration();*/

		/// mutate multiplier
		long u_multi = CAST_RP_EPANET->m_max_multiplier;
		long l_multi = CAST_RP_EPANET->m_min_multiplier;

		size_t pv_size = m_pv.get_variable().duration() / CAST_RP_EPANET->pattern_step();
		if (m_pv.get_variable().duration() % CAST_RP_EPANET->pattern_step() != 0)
			++pv_size;
		
		m_pv.get_variable().multiplier().resize(pv_size);

		std::vector<float> vec_r1(r1->get_variable().multiplier());
		std::vector<float> vec_r2(r2->get_variable().multiplier());
		std::vector<float> vec_r3(r3->get_variable().multiplier());
		vec_r1.resize(pv_size);
		vec_r2.resize(pv_size);
		vec_r3.resize(pv_size);
		for (auto&i : vec_r1)
			if (i == 0)
				i = l_multi;
		for (auto&i : vec_r2)
			if (i == 0)
				i = l_multi;
		for (auto&i : vec_r3)
			if (i == 0)
				i = l_multi;

		std::vector<float> vec_r4;
		std::vector<float> vec_r5;
		if (r4&&r5) {
			vec_r4 = r4->get_variable().multiplier();
			vec_r5 = r5->get_variable().multiplier();
			vec_r4.resize(pv_size);
			vec_r5.resize(pv_size);
			for (auto&i : vec_r4)
				if (i == 0)
					i = l_multi;
			for (auto&i : vec_r5)
				if (i == 0)
					i = l_multi;
		}

		for (size_t i = 0; i < pv_size; ++i) {
			m_pv.get_variable().multiplier()[i] = (vec_r1[i]) + F*((vec_r2[i]) - (vec_r3[i]));
			if (r4&&r5) m_pv.get_variable().multiplier()[i] += F*((vec_r4[i]) - (vec_r5[i]));

			if ((m_pv.get_variable().multiplier()[i]) > u_multi) {
				m_pv.get_variable().multiplier()[i] = ((vec_r1[i]) + u_multi) / 2;
			}
			else if ((m_pv.get_variable().multiplier()[i]) < l_multi) {
				m_pv.get_variable().multiplier()[i] = ((vec_r1[i]) + l_multi) / 2;
			}
		}

	}
	void ADOPT_individual::recombine(double CR) {
		// sort: index (0), duration (1), start_time (2), multiplier (3).
		m_pu = m_pv;
		
		size_t pv_size_multiplier = m_pv.get_variable().multiplier().size();
		size_t size = pv_size_multiplier;
		if (get_variable().multiplier().size() < pv_size_multiplier) {
			size = get_variable().multiplier().size();
		}
		size_t I = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<size_t>(0, pv_size_multiplier);
		for (size_t i = 0; i < size; ++i) {
			double p = global::ms_global->m_uniform[caller::Algorithm]->next();
			if (p <= CR || i == I) {
				m_pu.get_variable().multiplier()[i] = m_pv.get_variable().multiplier()[i];
			}
			else {
				m_pu.get_variable().multiplier()[i] = get_variable().multiplier()[i];
			}
		}
		/*size_t dim = 4;
		double p;
		size_t I = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<size_t>(0, dim);

		switch (I)
		{
		case 0: 
			m_pu.get_variable().index() = m_pv.get_variable().index();

			p = global::ms_global->m_uniform[caller::Algorithm]->next();
			if (p <= CR) { 
				m_pu.get_variable().duration() = m_pv.get_variable().duration(); 
				m_pu.get_variable().multiplier() = m_pv.get_variable().multiplier();
			}
			else {
				m_pu.get_variable().duration() = get_variable().duration();
				m_pu.get_variable().multiplier() = get_variable().multiplier();
			}

			p = global::ms_global->m_uniform[caller::Algorithm]->next();
			if (p <= CR) m_pu.get_variable().start_time() = m_pv.get_variable().start_time();
			else m_pu.get_variable().start_time() = get_variable().start_time();

			break;
		case 1:
		case 3:
			m_pu.get_variable().duration() = m_pv.get_variable().duration();
			m_pu.get_variable().multiplier() = m_pv.get_variable().multiplier();

			p = global::ms_global->m_uniform[caller::Algorithm]->next();
			if (p <= CR) m_pu.get_variable().index() = m_pv.get_variable().index();
			else m_pu.get_variable().index() = get_variable().index();

			p = global::ms_global->m_uniform[caller::Algorithm]->next();
			if (p <= CR) m_pu.get_variable().start_time() = m_pv.get_variable().start_time();
			else m_pu.get_variable().start_time() = get_variable().start_time();

			break;
		case 2:
			m_pu.get_variable().start_time() = m_pv.get_variable().start_time();

			p = global::ms_global->m_uniform[caller::Algorithm]->next();
			if (p <= CR) {
				m_pu.get_variable().duration() = m_pv.get_variable().duration();
				m_pu.get_variable().multiplier() = m_pv.get_variable().multiplier();
			}
			else {
				m_pu.get_variable().duration() = get_variable().duration();
				m_pu.get_variable().multiplier() = get_variable().multiplier();
			}

			p = global::ms_global->m_uniform[caller::Algorithm]->next();
			if (p <= CR) m_pu.get_variable().index() = m_pv.get_variable().index();
			else m_pu.get_variable().index() = get_variable().index();

			break;
		}*/
		
		
	}
	evaluation_tag ADOPT_individual::select(bool is_feasible) {
		evaluation_tag tag = m_pu.evaluate();
		if ((is_feasible && (m_pu_distance_fitness > m_distance_fitness)) || (!is_feasible && m_pu.get_objective()[0] < get_objective()[0])){
			m_var = m_pu.get_variable();
			m_obj = m_pu.get_objective();
			m_constraint_value = m_pu.constraint_value();
			m_impr = true;
		}
		else {
			m_impr = false;
		}
		return tag;
	}

	bool ADOPT_individual::same_location(ADOPT_individual & indi) {
		return m_var.index() == indi.get_variable().index();
	}
	void ADOPT_individual::initialize(int id) {
		individual::initialize(id);
		//m_flag = true;

		m_pu.get_variable() = get_variable();
		m_pu.get_objective() = get_objective();
		m_pu.constraint_value() = constraint_value();

		m_pv.get_variable() = get_variable();
		m_pv.get_objective() = get_objective();
		m_pv.constraint_value() = constraint_value();

		//m_pv = std::move(std::unique_ptr<solution_type>(new solution_type()));
	}
	solution<variable_epanet, real> & ADOPT_individual::trial() {
		return m_pu;
	}
}