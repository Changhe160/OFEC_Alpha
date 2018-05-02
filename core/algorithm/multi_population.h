/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class multi_population manages a set of populations with the same type of individuals.
*
*********************************************************************************/
#ifndef OFEC_MULTI_POPULATION_H
#define OFEC_MULTI_POPULATION_H

#include "population.h"

namespace OFEC {

	template<typename Population>
	class multi_population
	{
	public:
		using iterator_type = typename std::vector<std::unique_ptr<Population>>::iterator;

		multi_population() = default;
		virtual ~multi_population() {
			
		}

		multi_population(int n) :m_sub(n) {}
		multi_population(size_t n, size_t subsize) :m_sub(n) {
			for (auto& i : m_sub)
				i = std::move(std::unique_ptr<Population>(new Population(subsize, global::ms_global->m_problem->variable_size())));
		}

		void resize_objective(int n) {
			for (auto& i : m_sub)
				i.resize_objective(n);
		}
		void resize_variable(int n) {
			for (auto& i : m_sub)
				i.resize_variable(n);
		}

		iterator_type operator +(Population&& p) {
			m_sub.emplace_back(std::move(new Population(p)));
			return m_sub;
		}
		iterator_type operator +(Population& p) {
			m_sub.emplace_back(new Population(p));
			return m_sub;
		}
		void operator- (size_t id) {
			m_sub[id].release();
			if(id!=m_sub.size()-1)
				m_sub[id] = std::move(m_sub[m_sub.size()-1]);
			m_sub.pop_back();
		}

		Population& operator[](size_t i) {
			return *m_sub[i];
		}
		const Population& operator[](size_t i) const {
			return *m_sub[i];
		}

		const size_t& size() const{
			return m_sub.size();
		}

		void handle_evaluation_tag_all(evaluation_tag tag){}
		
	protected:
		std::vector<std::unique_ptr<Population>> m_sub;
		int m_maxsize;		// the maximum size of each sub-population
	};


}
#endif // !OFEC_MULTI_POPULATION_H