#pragma once
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
* class DE_individual is a class for differential evolution of individuals of an 
* evolutionary computation algorithm.
* 
*********************************************************************************/
#ifndef OFEC_DE_INDIVIDUAL_H
#define OFEC_DE_INDIVIDUAL_H
#include "../core/algorithm/individual.h"

namespace OFEC {
	template<class, class> class DE_population;
	//class CRDEPopulation;
	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare<ObjetiveType >>
	class DE_individual : public individual<VariableEncoding, ObjetiveType, ObjetiveCompare>
	{
		template<class, class> friend class DE_population;
		//friend class CRDEPopulation;
		using solution_type = solution<VariableEncoding, ObjetiveType, ObjetiveCompare>;
	protected:
		solution_type m_pv, m_pu;    // donor std::vector and trial std::vector, respectively.
	public:

		DE_individual() : individual() {}
		virtual ~DE_individual() {}
		DE_individual(const DE_individual &p) : individual(p),m_pv(p.m_pv),m_pu(p.m_pu) {}
		DE_individual(DE_individual &&p) : individual(std::move(p)), m_pv(std::move(p.m_pv)), m_pu(std::move(p.m_pu)) {}
		DE_individual(const solution_type &p) : individual(p), m_pv(p.m_pv), m_pu(p.m_pu) {}
		DE_individual(solution_type &&p) : individual(std::move(p)), m_pv(std::move(p.m_pv)), m_pu(std::move(p.m_pu)) {}

		DE_individual & operator=(const DE_individual &other);
		virtual void initialize(int id = 0, int idx = 0, int max_idx = 0);
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
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare<ObjetiveType >>
	DE_individual<VariableEncoding, ObjetiveType, ObjetiveCompare> & 
		DE_individual<VariableEncoding, ObjetiveType, ObjetiveCompare>::operator=(const DE_individual &rhs) {
		if (this == &rhs) return *this; 

		individual::operator=(rhs);
		m_pv = rhs.m_pv;
		m_pu = rhs.m_pu;
		return *this;
	}

	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare<ObjetiveType >>
	void DE_individual<VariableEncoding, ObjetiveType, ObjetiveCompare>::initialize(int id, int idx, int max_idx) {
		evaluation_tag tag = individual::initialize(id, idx, max_idx);
		//m_flag = true;
		//m_pv = self();
		//m_pu = self();
	}

	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare<ObjetiveType >>
	void DE_individual<VariableEncoding, ObjetiveType, ObjetiveCompare>::mutate(double F, 
		solution_type *r1,
		solution_type *r2,
		solution_type *r3,
		solution_type *r4,
		solution_type *r5) {

		double l, u;
		for (int i = 0; i<GET_NUM_DIM; i++) {
			//CAST_PROBLEM_CONT->getSearchRange(l, u, i);   
			m_pv.m_var[i] = (r1->m_var[i]) + F*((r2->m_var[i]) - (r3->m_var[i]));
			if (r4&&r5) m_pv.m_var[i] += F*((r4->m_var[i]) - (r5->m_var[i]));

			if ((m_pv.m_var[i])>u) {
				m_pv.m_var[i] = ((r1->m_var[i]) + u) / 2;
			}
			else if ((m_pv.m_var[i])<l) {
				m_pv.m_var[i] = ((r1->m_var[i]) + l) / 2;
			}

		}
	}

	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare<ObjetiveType >>
	void DE_individual<VariableEncoding, ObjetiveType, ObjetiveCompare>::recombine(double CR) {
		int I = global::ms_global->getRandInt(0, GET_NUM_DIM, Algorithm);

		for (int i = 0; i<GET_NUM_DIM; i++) {
			double p = global::ms_global->m_uniformAlg->next();
			if (p <= CR || i == I)     m_pu.m_var[i] = m_pv.m_var[i];
			else m_pu.m_var[i] = m_var[i];
		}

	}

	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare<ObjetiveType >>
	void DE_individual<VariableEncoding, ObjetiveType, ObjetiveCompare>::mutate(double F, const std::vector<int> &var, 
		solution_type *r1,
		solution_type *r2,
		solution_type *r3,
		solution_type *r4,
		solution_type *r5) {

		double l, u;
		for (auto &i : var) {
			//CAST_PROBLEM_CONT->getSearchRange(l, u, i);
			m_pv.m_var[i] = (r1->m_var[i]) + F*((r2->m_var[i]) - (r3->m_var[i]));
			if (r4&&r5) m_pv.m_var[i] += F*((r4->m_var[i]) - (r5->m_var[i]));

			if ((m_pv.m_var[i])>u) {
				m_pv.m_var[i] = ((r1->m_var[i]) + u) / 2;
			}
			else if ((m_pv.m_var[i])<l) {
				m_pv.m_var[i] = ((r1->m_var[i]) + l) / 2;
			}

		}
	}

	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare<ObjetiveType >>
	void DE_individual<VariableEncoding, ObjetiveType, ObjetiveCompare>::recombine(double CR, const std::vector<int> &var, int I) {

		for (auto &i : var) {
			double p = global::ms_global->m_uniformAlg->next();
			if (p <= CR || i == I)     m_pu.m_var[i] = m_pv.m_var[i];
			else m_pu.m_var[i] = m_var[i];
		}

	}

	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare<ObjetiveType >>
	evaluation_tag DE_individual<VariableEncoding, ObjetiveType, ObjetiveCompare>::select() {
		evaluation_tag tag = m_pu.evaluate();
		if (m_pu > self()) {
			self() = m_pu;
			m_impr = true;
		}
		else {
			m_impr = false;
		}
		return tag;
	}

	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare<ObjetiveType >>
	evaluation_tag DE_individual<VariableEncoding, ObjetiveType, ObjetiveCompare>::select(const std::vector<int> &var, solution_type &best) {
		int dim = GET_NUM_DIM;
		solution_type t(m_pu);
		for (int i = 0, j = 0; i < dim; i++) {
			if (find(var.begin(), var.end(), i) == var.end()) {
				t.m_var[i] = best.m_var[i];
			}
			else {
				t.m_var[i] = m_pu.m_var[j++];
			}
		}

		evaluation_tag tag = t.evaluate();
		m_pu.m_obj.m_o = t.m_obj.m_o;
		if (t > self()) {
			self() = m_pu;
			if (t > best)
				best = t;
		}

		return tag;
	}

	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare<ObjetiveType >>
	solution_type& DE_individual<VariableEncoding, ObjetiveType, ObjetiveCompare>::trial() {
		return m_pu;
	}

}
#endif // !OFEC_DE_INDIVIDUAL_H
