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

namespace OFEC {

	template<typename Population>
	class multi_population {
	public:
		using iterator_type = typename std::vector<std::unique_ptr<Population>>::iterator;
	public:
		multi_population() = default;

		virtual ~multi_population() {}

		multi_population(int n) : m_sub(n) {}

		template<typename ... Args>
		multi_population(size_t n, size_t subsize, Args&& ...args) : m_sub(n) {
			for (auto& i : m_sub)
				i.reset(new Population(subsize, std::forward<Args>(args)...));
		}

		void resize_objective(int n) {
			for (auto& i : m_sub)
				i.resize_objective(n);
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
			for (size_t i = id; i < m_sub.size() - 1; ++i)
				m_sub[i] = m_sub[i + 1];
			m_sub.pop_back();
		}

		Population& operator[](size_t i) {
			return *m_sub[i];
		}
		
		const Population& operator[](size_t i) const {
			return *m_sub[i];
		}

		size_t size() const {
			return m_sub.size();
		}

		void pop_back() {
			m_sub.pop_back();
		}
		
	protected:
		std::vector<std::unique_ptr<Population>> m_sub;
		int m_maxsize;		// the maximum size of each sub-population
	};
}
#endif // !OFEC_MULTI_POPULATION_H