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
*-------------------------------------------------------------------------------
* class DEindividual is a class for differential evolution of individuals of an 
* evolutionary computation algorithm.
*********************************************************************************/
// updated Mar 28, 2018 by Li Zhou


#ifndef OFEC_DEINDIVIDUAL_H
#define OFEC_DEINDIVIDUAL_H
#include "../../../core/algorithm/individual.h"
#include "../../../core/problem/continuous/continuous.h"
#include "../../problem/realworld/EPANET/epa_encoding.h"

namespace OFEC {
	namespace DE {
		//template<class, class> class DE_population;

		template<typename VariableEncoding = variable<real>,
			typename ObjetiveType = real>
			class individual : public OFEC::individual<VariableEncoding, ObjetiveType>
		{
		public:
			//template<class, class> friend class DE_population;

			using solution_type = solution<VariableEncoding, ObjetiveType>;
			using individual_type = OFEC::individual<VariableEncoding, ObjetiveType>;
		protected:
			solution_type m_pv, m_pu;    // donor std::vector and trial std::vector, respectively.
		public:
			template<typename ... Args>
			individual(size_t no, Args&& ... args) : individual_type(no, std::forward<Args>(args)...), m_pv(no, std::forward<Args>(args)...), m_pu(no, std::forward<Args>(args)...) {}
			individual(const individual &p) : individual_type(p), m_pv(p.m_pv), m_pu(p.m_pu) {}
			individual(individual &&p) : individual_type(std::move(p)), m_pv(std::move(p.m_pv)), m_pu(std::move(p.m_pu)) {}
			individual(const solution_type &p) : individual_type(p), m_pv(p.objective_size(), p.variable_size()), m_pu(p.objective_size(), p.variable_size()) {}
			individual(solution_type &&p) : individual_type(std::move(p)), m_pv(variable_size()), m_pu(variable_size()) {}

			individual & operator=(const individual &other);
			individual & operator=(individual &&other);
			void initialize(int id);

			virtual void mutate(double F, solution_type *r1,
				solution_type *r2,
				solution_type *r3,
				solution_type *r4 = 0,
				solution_type *r5 = 0);
			virtual void recombine(double CR);
			virtual evaluation_tag select();
			solution_type& trial();

			void recombine(double CR, const std::vector<int> &var, int I);
			void mutate(double F, const std::vector<int> &var, solution_type *r1,
				solution_type *r2,
				solution_type *r3,
				solution_type *r4 = 0,
				solution_type *r5 = 0);
			evaluation_tag select(const std::vector<int> &var, solution_type &best);
		};

		/*Definition*/

		template<typename VariableEncoding = variable<real>,
			typename ObjetiveType = real>
			individual<VariableEncoding, ObjetiveType> &
			individual<VariableEncoding, ObjetiveType>::operator=(const individual &rhs) {
			if (this == &rhs) return *this;

			individual_type::operator=(rhs);
			m_pv = rhs.m_pv;
			m_pu = rhs.m_pu;
			return *this;
		}
		template<typename VariableEncoding = variable<real>,
			typename ObjetiveType = real>
			individual<VariableEncoding, ObjetiveType> &
			individual<VariableEncoding, ObjetiveType>::operator=(individual &&rhs) {
			if (this == &rhs) return *this;

			individual_type::operator=(std::move(rhs));
			m_pv = std::move(rhs.m_pv);
			m_pu = std::move(rhs.m_pu);
			return *this;
		}
		template<typename VariableEncoding = variable<real>,
			typename ObjetiveType = real>
			void individual<VariableEncoding, ObjetiveType>::initialize(int id) {
			individual_type::initialize(id);
			//m_flag = true;

			m_pu.get_variable() = get_variable();
			m_pu.get_objective() = get_objective();
			m_pu.constraint_value() = constraint_value();

			m_pv.get_variable() = get_variable();
			m_pv.get_objective() = get_objective();
			m_pv.constraint_value() = constraint_value();

			//m_pv = std::move(std::unique_ptr<solution_type>(new solution_type()));
		}

		template<typename VariableEncoding = variable<real>,
			typename ObjetiveType = real>
			void individual<VariableEncoding, ObjetiveType>::mutate(double F,
				solution_type *r1,
				solution_type *r2,
				solution_type *r3,
				solution_type *r4,
				solution_type *r5) {
			
			real l, u;
			for (size_t i = 0; i < m_pv.get_variable().size(); ++i) {
				l = CONTINOUS_CAST->range(i).first;
				u = CONTINOUS_CAST->range(i).second;
				m_pv.get_variable()[i] = (r1->get_variable()[i]) + F*((r2->get_variable()[i]) - (r3->get_variable()[i]));
				if (r4&&r5) m_pv.get_variable()[i] += F*((r4->get_variable()[i]) - (r5->get_variable()[i]));

				if ((m_pv.get_variable()[i]) > u) {
					m_pv.get_variable()[i] = ((r1->get_variable()[i]) + u) / 2;
				}
				else if ((m_pv.get_variable()[i]) < l) {
					m_pv.get_variable()[i] = ((r1->get_variable()[i]) + l) / 2;
				}

			}
		}

		template<typename VariableEncoding = variable<real>,
			typename ObjetiveType = real>
			void individual<VariableEncoding, ObjetiveType>::recombine(double CR) {
			
			size_t dim = m_var.size();
			int I = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)dim);

			for (size_t i = 0; i < dim; ++i) {
				double p = global::ms_global->m_uniform[caller::Algorithm]->next();
				if (p <= CR || i == I)     m_pu.get_variable()[i] = m_pv.get_variable()[i];
				else m_pu.get_variable()[i] = get_variable()[i];
			}
			
			
		}

		template<typename VariableEncoding = variable<real>,
			typename ObjetiveType = real>
			void individual<VariableEncoding, ObjetiveType>::mutate(double F, const std::vector<int> &var,
				solution_type *r1,
				solution_type *r2,
				solution_type *r3,
				solution_type *r4,
				solution_type *r5) {
			real l, u;
			for (auto &i : var) {
				l = CONTINOUS_CAST->range(i).first;
				u = CONTINOUS_CAST->range(i).second;
				m_pv.get_variable()[i] = (r1->get_variable()[i]) + F*((r2->get_variable()[i]) - (r3->get_variable()[i]));
				if (r4&&r5) m_pv.get_variable()[i] += F*((r4->get_variable()[i]) - (r5->get_variable()[i]));

				if ((m_pv.get_variable()[i]) > u) {
					m_pv.get_variable()[i] = ((r1->get_variable()[i]) + u) / 2;
				}
				else if ((m_pv.get_variable()[i]) < l) {
					m_pv.get_variable()[i] = ((r1->get_variable()[i]) + l) / 2;
				}

			}
			
		}

		template<typename VariableEncoding = variable<real>,
			typename ObjetiveType = real>
			void individual<VariableEncoding, ObjetiveType>::recombine(double CR, const std::vector<int> &var, int I) {

			for (auto &i : var) {
				double p = global::ms_global->m_uniform[caller::Algorithm]->next();
				if (p <= CR || i == I)     m_pu.get_variable()[i] = m_pv.get_variable()[i];
				else m_pu.get_variable()[i] = get_variable()[i];
			}

		}

		template<typename VariableEncoding = variable<real>,
			typename ObjetiveType = real>
			evaluation_tag individual<VariableEncoding, ObjetiveType>::select() {
			evaluation_tag tag = m_pu.evaluate();
			if (m_pu.dominate(*this)) {
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

		template<typename VariableEncoding = variable<real>,
			typename ObjetiveType = real>
			evaluation_tag individual<VariableEncoding, ObjetiveType>::select(const std::vector<int> &var, solution_type &best) {
			int dim = m_var.size();
			solution_type t(m_pu);
			for (int i = 0, j = 0; i < dim; ++i) {
				if (find(var.begin(), var.end(), i) == var.end()) {
					t.get_variable()[i] = best.get_variable()[i];
				}
				else {
					t.get_variable()[i] = m_pu.get_variable()[j++];
				}
			}

			evaluation_tag tag = t.evaluate();
			m_pu.m_obj.m_o = t.m_obj.m_o;
			if (m_pu.dominate(self())) {
				self() = m_pu;
				if (t > best)
					best = t;
			}

			return tag;
		}

		template<typename VariableEncoding = variable<real>,
			typename ObjetiveType = real>
			solution<VariableEncoding, ObjetiveType> & individual<VariableEncoding, ObjetiveType>::trial() {
			return m_pu;
		}
	}
}
#endif // !OFEC_DEINDIVIDUAL_H
