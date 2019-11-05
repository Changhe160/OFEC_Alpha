/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*********************************************************************************/
// updated Jan 21, 2019 by Li Zhou

#ifndef OFEC_CC_FRAMEWORK_H
#define OFEC_CC_FRAMEWORK_H

#include<memory>

namespace OFEC {
	template <typename Population1, typename Population2, typename Individual>
	class CC_framework    // just for epanet
	{
	public:
		/*friend class ADOPT<Population1, Population2, Individual>;
		friend class AMP<Population1, Population2, Individual>;
		friend class LRM_ADOPT<Population1, Population2, Individual>;*/

		enum class fill_type { random, best };
		std::unique_ptr<Population1> m_first_pop;
		std::unique_ptr<Population2> m_second_pop;
		std::unique_ptr<Individual> m_best;
		std::vector<real> m_history_best_obj;

		std::vector<std::vector<float>> m_history_mass;
	public:
		CC_framework() = default;
		CC_framework(size_t size) {
			m_first_pop = std::move(std::unique_ptr<Population1>(new Population1(size)));
			m_second_pop = std::move(std::unique_ptr<Population2>(new Population2(size)));
			size_t num_obj = global::ms_global->m_problem->objective_size();
			size_t num_con = global::ms_global->m_problem->num_constraints();
			m_best = std::move(std::unique_ptr<Individual>(new Individual(num_obj, num_con)));
			m_best->variable().index() = -1;
		}
		virtual ~CC_framework() {

		}
		
		CC_framework & operator=(CC_framework & cc) {
			/*this->m_first_pop->reset();
			this->m_second_pop->reset();*/
			*m_first_pop = *cc.m_first_pop;
			*m_second_pop = *cc.m_second_pop;
			*m_best = *cc.m_best;
			m_history_best_obj = cc.m_history_best_obj;

			return *this;
		}

		CC_framework & operator=(CC_framework && cc) {
			/*this->m_first_pop->reset();
			this->m_second_pop->reset();*/
			*m_first_pop = std::move(*cc.m_first_pop);
			*m_second_pop = std::move(*cc.m_second_pop);
			*m_best = std::move(*cc.m_best);
			m_history_best_obj = std::move(cc.m_history_best_obj);

			return *this;
		}

		void fill_each(fill_type type);
		void update_best();

		void initialize_new_pop();
		bool is_stable_obj();
		bool is_stable_mass();

		void record_history_mass();

		void clear_history() {
			m_history_best_obj.clear();
		}

	};

	template <typename Population1, typename Population2, typename Individual>
	void CC_framework<Population1, Population2, Individual>::fill_each(fill_type type) {
		if (type == fill_type::random) {
			size_t I = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<size_t>(0, m_second_pop->size());
			m_first_pop->fill_solution((*m_second_pop)[I]);
			I = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<size_t>(0, m_second_pop->size());
			m_second_pop->fill_solution((*m_first_pop)[I]);
		}
		else if (type == fill_type::best) {
			m_first_pop->update_flag_best() = false;
			m_second_pop->update_flag_best() = false;
			m_first_pop->fill_solution(Individual(*(m_second_pop->best()[0])));
			m_second_pop->fill_solution(Individual(*(m_first_pop->best()[0])));
		}

		m_first_pop->evaluate();
		m_second_pop->evaluate();
		m_first_pop->update_flag_best() = false;
		m_second_pop->update_flag_best() = false;
		update_best();
		m_history_best_obj.push_back(m_best->objective()[0]);
	}

	template <typename Population1, typename Population2, typename Individual>
	void CC_framework<Population1, Population2, Individual>::update_best() {
		m_first_pop->update_flag_best() = false;
		m_second_pop->update_flag_best() = false;

		Individual temp(1, 1);
		/*temp = *(m_second_pop->best()[0]);
		temp.variable().index() = (*(m_first_pop->best()[0])).variable().index();
		strcpy(temp.variable().location(), (*(m_first_pop->best()[0])).variable().location());*/

		temp = *(m_first_pop->best()[0]);
		temp.variable().duration() = (*(m_first_pop->best()[0])).variable().duration();
		temp.variable().start_time() = (*(m_first_pop->best()[0])).variable().start_time();
		temp.variable().multiplier() = (*(m_first_pop->best()[0])).variable().multiplier();

		temp.evaluate();

		if (m_best->variable().index() == -1) {
			*m_best = temp;
		}
		else {
			m_best->evaluate();
			if (temp.objective()[0] < m_best->objective()[0])
				*m_best = temp;
		}
	}

	template <typename Population1, typename Population2, typename Individual>
	void CC_framework<Population1, Population2, Individual>::initialize_new_pop() {
		m_first_pop->initialize();
		m_second_pop->initialize();
		update_best();
		/*if (m_best->objective()[0] > benchmark) {
			for (size_t i = 0; i < m_first_pop->size(); ++i) {
				(*m_first_pop)[i].objective()[0] = benchmark;
				(*m_second_pop)[i].objective()[0] = benchmark;
			}
		}*/
	}

	template <typename Population1, typename Population2, typename Individual>
	bool CC_framework<Population1, Population2, Individual>::is_stable_obj() {
		size_t standard = 5;
		if (m_history_best_obj.size() < standard) return false;
		int i = m_history_best_obj.size() - 1;
		int count = 0;
		while (m_history_best_obj[i] == m_history_best_obj[i - 1]) {
			++count;
			--i;
			if (i == 0) break;
		}
		return count >= standard - 1;
	}

	template <typename Population1, typename Population2, typename Individual>
	void CC_framework<Population1, Population2, Individual>::record_history_mass() {
		m_history_mass.push_back(m_best->variable().multiplier());
	}

	template <typename Population1, typename Population2, typename Individual>
	bool CC_framework<Population1, Population2, Individual>::is_stable_mass() {
		size_t standard = 3;
		if (m_history_mass.size() < standard) return false;
		int i = m_history_mass.size() - 1;
		int count = 0;
		while (m_history_mass[i] == m_history_mass[i - 1]) {
			++count;
			--i;
			if (i == 0) break;
		}
		return count >= standard - 1;
	}
}

#endif