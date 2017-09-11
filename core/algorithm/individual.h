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
* class individual is a base class for all types of individuals of an evolutionary
* computation algorithm
*
*********************************************************************************/

#ifndef OFEC_INDIVIDUAL_H
#define OFEC_INDIVIDUAL_H

#include "solution.h"

namespace OFEC {
	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare<ObjetiveType>>
	class individual:public solution<VariableEncoding, ObjetiveType, ObjetiveCompare> {
	public:
		using solution_type = solution<VariableEncoding, ObjetiveType, ObjetiveCompare>;

		template<typename ... Args>
		individual(size_t no, Args&& ... args) :solution(no, args) { }

		individual(solution_type &&s):solution(std::move(s)){}

		individual(const solution_type &s) :solution(s) {}
		
		explicit individual() :solution() {}
		virtual void initialize(int id) {
			m_id = id;
			solution::initialize();
		}

		void set_id(int id)noexcept{
			m_id = id;
		}
		void set_fitness(double value) noexcept {
			m_fitness = value;
		}
		void set_type(int type)noexcept {
			m_type = type;
		}
		void set_rank(int value) noexcept {
			m_ranking = value;
		}
		void set_improve_flag(bool value) noexcept {
			m_impr = value;
		}
		void set_active_flag(bool value) noexcept {
			m_active = value;
		}

		int id()const noexcept {
			return m_id;
		}
		double fitness()const noexcept {
			return m_fitness;
		}
		int type()const noexcept {
			return m_type;
		}
		int rank() const noexcept {
			return m_ranking;
		}
		bool is_improved()const noexcept {
			return m_impr;
		}
		bool is_active()const noexcept {
			return m_active;
		}

		individual(const individual& rhs) :solution(rhs), m_fitness(rhs.m_fitness), m_id(rhs.m_id), m_ranking(rhs.m_ranking),
			m_type(rhs.m_type), m_impr(rhs.m_impr), m_active(rhs.m_active) {

		}

		individual(individual&& rhs) :solution(std::move(rhs)), m_fitness(std::move(rhs.m_fitness)), m_id(std::move(rhs.m_id)), 
			m_ranking(std::move(rhs.m_ranking)),m_type(std::move(rhs.m_type)), m_impr(std::move(rhs.m_impr)), m_active(std::move(rhs.m_active)) {

		}


		individual& operator=(const individual& rhs) {
			if (this == &rhs) return *this;
			solution::operator=(rhs);
			m_fitness = rhs.m_fitness;
			m_id = rhs.m_id;
			m_ranking = rhs.m_ranking;
			m_type = rhs.m_type;
			m_impr = rhs.m_impr;
			m_active = rhs.m_active;
			return *this;
		}

		individual& operator=(individual&& rhs) {
		
			solution::operator=(std::move(rhs));
			m_fitness = std::move(rhs.m_fitness);
			m_id = std::move(rhs.m_id);
			m_ranking = std::move(rhs.m_ranking);
			m_type = std::move(rhs.m_type);
			m_impr = std::move(rhs.m_impr);
			m_active = std::move(rhs.m_active);

			return *this;
		}

	protected:
		double m_fitness;
		int m_id, m_ranking = -1, m_type;
		bool m_impr=false, m_active = true;
	};

}
#endif // !OFEC_INDIVIDUAL_H

