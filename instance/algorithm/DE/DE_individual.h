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

namespace OFEC {
	namespace DE {
		//template<class, class> class DE_population;

		class individual : public OFEC::individual<variable_vector<real>, real>	{
		public:
			using individual_type = OFEC::individual<variable_vector<real>, real>;
		protected:
			solution_type m_pv, m_pu;    // donor std::vector and trial std::vector, respectively.
		public:
			template<typename ... Args>
			individual(size_t no, Args&& ... args) : individual_type(no, std::forward<Args>(args)...), m_pv(no, std::forward<Args>(args)...), m_pu(no, std::forward<Args>(args)...) {}
			individual(const individual &p) : individual_type(p), m_pv(p.m_pv), m_pu(p.m_pu) {}
			individual(individual &&p) : individual_type(std::move(p)), m_pv(std::move(p.m_pv)), m_pu(std::move(p.m_pu)) {}
	
			individual & operator=(const individual &rhs) {				
					if (this == &rhs) return *this;

					individual_type::operator=(rhs);
					m_pv = rhs.m_pv;
					m_pu = rhs.m_pu;
					return *this;	
			}
			individual & operator=(individual &&rhs) {
				if (this == &rhs) return *this;

				individual_type::operator=(std::move(rhs));
				m_pv = std::move(rhs.m_pv);
				m_pu = std::move(rhs.m_pu);
				return *this;
			}
			void initialize(int id) {
				individual_type::initialize(id);
				m_pu = m_pv = solut();
			}

			virtual void mutate(double F, solution_type *r1, solution_type *r2,	
				solution_type *r3,	solution_type *r4 = 0,	solution_type *r5 = 0) {
				real l, u;
				for (size_t i = 0; i < m_pv.variable().size(); ++i) {
					l = CONTINOUS_CAST->range(i).first;
					u = CONTINOUS_CAST->range(i).second;
					m_pv.variable()[i] = (r1->variable()[i]) + F*((r2->variable()[i]) - (r3->variable()[i]));
					if (r4&&r5) m_pv.variable()[i] += F*((r4->variable()[i]) - (r5->variable()[i]));

					if ((m_pv.variable()[i]) > u) {
						m_pv.variable()[i] = ((r1->variable()[i]) + u) / 2;
					}
					else if ((m_pv.variable()[i]) < l) {
						m_pv.variable()[i] = ((r1->variable()[i]) + l) / 2;
					}

				}
			}
			virtual void recombine(double CR) {
				size_t dim = m_var.size();
				int I = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)dim);

				for (size_t i = 0; i < dim; ++i) {
					double p = global::ms_global->m_uniform[caller::Algorithm]->next();
					if (p <= CR || i == I)     m_pu.variable()[i] = m_pv.variable()[i];
					else m_pu.variable()[i] = variable()[i];
				}

			}

			virtual evaluation_tag select() {
				evaluation_tag tag = m_pu.evaluate();
				if (m_pu.dominate(*this)) {
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
			solution_type& trial() {
				return m_pu;
			}

			void recombine(double CR, const std::vector<int> &var, int I) {

				for (auto &i : var) {
					double p = global::ms_global->m_uniform[caller::Algorithm]->next();
					if (p <= CR || i == I)     m_pu.variable()[i] = m_pv.variable()[i];
					else m_pu.variable()[i] = variable()[i];
				}

			}
			void mutate(double F, const std::vector<int> &var, solution_type *r1,
				solution_type *r2,	solution_type *r3,	solution_type *r4 = 0,	solution_type *r5 = 0) {
				real l, u;
				for (auto &i : var) {
					l = CONTINOUS_CAST->range(i).first;
					u = CONTINOUS_CAST->range(i).second;
					m_pv.variable()[i] = (r1->variable()[i]) + F*((r2->variable()[i]) - (r3->variable()[i]));
					if (r4&&r5) m_pv.variable()[i] += F*((r4->variable()[i]) - (r5->variable()[i]));

					if ((m_pv.variable()[i]) > u) {
						m_pv.variable()[i] = ((r1->variable()[i]) + u) / 2;
					}
					else if ((m_pv.variable()[i]) < l) {
						m_pv.variable()[i] = ((r1->variable()[i]) + l) / 2;
					}

				}
			}
		};

	}
}
#endif // !OFEC_DEINDIVIDUAL_H
