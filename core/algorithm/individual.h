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
* class individual is an abstract class for all types of individuals of an evolutionary
* computation algorithm
*
*********************************************************************************/

#ifndef OFEC_INDIVIDUAL_H
#define OFEC_INDIVIDUAL_H

#include "solution.h"

namespace OFEC {
	template<typename VariableEncoding = variable_vector<real>,
		typename ObjetiveType = real>
		class individual :public solution<VariableEncoding, ObjetiveType> {
		public:
			using solution_type = solution<VariableEncoding, ObjetiveType>;
			individual() = default;
			template<typename ... Args>
			individual(size_t num_obj, size_t num_con, Args&& ... args) : solution_type(num_obj, num_con, std::forward<Args>(args)...) {}
			individual(solution_type &&s) : solution_type(std::move(s)) {}
			individual(const solution_type &s) : solution_type(s) {}

			virtual void initialize(int id) {
				m_id = id;
				solution_type::initialize();
			}

			void set_id(int id)noexcept {
				m_id = id;
			}
			void set_fitness(real value) noexcept {
				m_fitness = value;
			}
			void set_type(int type)noexcept {
				m_type = type;
			}
			void set_rank(int value) noexcept {
				m_ranking = value;
			}
			void set_improved_flag(bool value) noexcept {
				m_improved = value;
			}
			void set_active_flag(bool value) noexcept {
				m_active = value;
			}

			int id()const noexcept {
				return m_id;
			}
			real fitness()const noexcept {
				return m_fitness;
			}
			int type()const noexcept {
				return m_type;
			}
			int rank() const noexcept {
				return m_ranking;
			}
			bool is_improved()const noexcept {
				return m_improved;
			}
			bool is_active()const noexcept {
				return m_active;
			}
			individual(const individual& rhs) = default;
			individual(individual&& rhs) = default;
			individual& operator=(const individual &rhs) = default;
			individual& operator=(individual &&rhs) = default;

		protected:
			real m_fitness;
			int m_id, m_ranking = -1, m_type;
			bool m_improved = false, m_active = true;
	};
}
#endif // !OFEC_INDIVIDUAL_H
