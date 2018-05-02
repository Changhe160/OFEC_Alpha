#include "ADOPT.h"

namespace OFEC {
	ADOPT::ADOPT(param_map & v) : population(0, global::ms_global->m_problem->variable_size()), \
		m_sub_population(v.at("numSubPop"), v.at("subPopSize")),m_tar(1.2)
	{

		for (size_t i = 0; i < m_sub_population.size(); ++i) {  // initialize subpopulation solution
			m_alternative.emplace_back(std::make_pair(new ADOPT_individual(*(m_sub_population[i].best()[0])),i));
		}
	}
	evaluation_tag ADOPT::run_() {
		evaluation_tag tag = evaluation_tag::Normal;

		while (tag != evaluation_tag::Terminate) {
			double best = problem::get_sofar_best<solution<variable_epanet>>(0)->get_objective()[0];
			size_t total_evaluations = CAST_RP_EPANET->total_evaluations();
			std::cout << m_iter << "  phase: " << CAST_RP_EPANET->phase() << "  total_eval: " << total_evaluations << "  alternative: " << m_alternative.size() << std::endl;
			for (auto &i : m_alternative)
				std::cout << m_sub_population[i.second].best()[0]->get_variable().location() << " ";
			std::cout << std::endl;
			//measure::ms_measure->record(global::ms_global.get(), m_iter, m_alternative.size()); 


			tag = evolve();

			if(m_iter % 3 == 0){
				
				std::vector<size_t> sub_idx;
				for (auto &i : m_alternative)
					sub_idx.emplace_back(i.second);
				m_alternative.clear();
				for (size_t i = 0; i < sub_idx.size(); ++i) {
					update_alternative_solution(sub_idx[i], *(m_sub_population[sub_idx[i]].best()[0]));
				}
				if (m_alternative.size() == 1) break;
				m_tar *= 0.43;
				CAST_RP_EPANET->phase_next();
				if (CAST_RP_EPANET->is_time_out()) break;
			}

		}
		//solution<variable_epanet>* best_var = problem::get_sofar_best<solution<variable_epanet>>(0);
		for (size_t i = 0; i < m_alternative.size(); ++i) {
			std::cout << "best solution "<< i+1 << ": " << std::endl;
			std::cout << "location: " << m_alternative[i].first->get_variable().location() << std::endl;
			std::cout << "start time: " << m_alternative[i].first->get_variable().start_time() << std::endl;
			std::cout << "duration: " << m_alternative[i].first->get_variable().duration() << std::endl;
			std::cout << "multiplier: " << std::endl;
			for (auto &i : m_alternative[i].first->get_variable().multiplier())
				std::cout << i << std::endl;
			std::cout << m_alternative[i].first->get_objective()[0] << std::endl;
			measure::ms_measure->record(global::ms_global.get(), i+1, m_alternative.size());
		}
		
		return tag;
	}
	evaluation_tag ADOPT::evolve() {
		evaluation_tag tag = evaluation_tag::Normal;
		int count = 0;
		for (size_t i = 0; i < m_alternative.size(); ++i) {
			size_t idx = m_alternative[i].second;
			m_sub_population[idx].update_probability();
			//m_sub_population[idx].update_archive(*m_sub_population[idx].best()[0]);
			m_sub_population[idx].mutate();
			m_sub_population[idx].recombine();
		
			if (i > 0 && m_sub_population[idx].is_feasible_population(m_tar)) {
				for (size_t j = 0; j < m_sub_population[idx].size(); ++j) {
					update_distance(idx, m_sub_population[idx][j]);   // for selection by distance
				}
			}
			m_sub_population[idx].select(m_sub_population[idx].m_is_feasible);
		}
		++m_iter;
		if (m_iter == 500) return evaluation_tag::Terminate;
		return tag;
	}
	
	void ADOPT::update_distance(size_t idx, ADOPT_individual & individual) {  
		float min_distance;
		for (size_t i = 0; i < m_alternative.size(); ++i) {
			size_t sub_idx = m_alternative[i].second;
			if (sub_idx == idx) continue;
			float dis = CAST_RP_EPANET->calculate_distance(m_sub_population[sub_idx].best()[0]->get_variable().index(), individual.get_variable().index());
			if (i == 0) min_distance = dis;
			else if (min_distance > dis) min_distance = dis;
		}
		individual.m_distance_fitness = min_distance;

		for (size_t i = 0; i < m_alternative.size(); ++i) {
			size_t sub_idx = m_alternative[i].second;
			if (sub_idx == idx) continue;
			float dis = CAST_RP_EPANET->calculate_distance(m_sub_population[sub_idx].best()[0]->get_variable().index(), individual.trial().get_variable().index());
			if (i == 0) min_distance = dis;
			else if (min_distance > dis) min_distance = dis;
		}
		individual.m_pu_distance_fitness = min_distance;

	}
	bool ADOPT::update_alternative_solution(size_t idx, ADOPT_individual & indi) {
		for (auto& i : m_alternative) {
			if (i.first->same_location(indi)) {
				if (indi.dominate(*(i.first))) {
					//i.first.release();
					i.first.reset(new ADOPT_individual(indi));
					i.second = idx;
				}
				return false;
			}
		}
		m_alternative.emplace_back(std::make_pair(new ADOPT_individual(indi), idx));
		return true;
	}
}