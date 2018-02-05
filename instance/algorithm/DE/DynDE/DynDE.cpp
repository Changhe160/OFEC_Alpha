/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 25 Sep. 2014
// Last modified:

#include "DynDE.h"
namespace OFEC {

	DynDE::DynDE(param_map &v) :DEpopulation(v[param_popSize], v[param_numDim]), multi_population(v[param_popSize]/v[param_subPopSize], v[param_subPopSize]), \
		m_M(v[param_popSize] / v[param_subPopSize]), m_r_excl(v[param_exlRadius]) {
		//set_default(v);

		m_parameter << "M(N,Nbrownian):" << m_M << "(" << 5 << "," << 5 << "); Rexcl:" << m_r_excl << "; Rcloud:" << 0.2;
	}


	void DynDE::set_default(param_map &v) {
		// for global population size =100
		if ((v[param_popSize]) == 100) m_M = 10;
		else m_M = (v[param_popSize]) / 10;

		/*double u, l;
		int d = GET_NUM_DIM;
		int peaks = 0;
		if (CAST_PROBLEM_DYN)peaks = CAST_PROBLEM_DYN->getNumberofPeak();
		else if (CAST_PROBLEM_DYN_ONEPEAK) peaks = CAST_PROBLEM_DYN_ONEPEAK->getNumPeak();
		CAST_PROBLEM_CONT->getSearchRange(l, u, 0);


		if ((v[param_exlRadius]) <= 0.0)
			m_r_excl = 0.5*(u - l) / pow((double)peaks, 1. / d);
		else m_r_excl = (v[param_exlRadius]);*/

	}

	void DynDE::exclude() {
		for (size_t i = 0; i < m_M; ++i) {
			for (size_t j = i + 1; j < m_M; ++j) {
				if (m_sub[i]->m_flag == false && m_sub[j]->m_flag == false && m_sub[i]->best()[0]->self().variable_distance(m_sub[j]->best()[0]->self()) < m_r_excl) {
					if (m_sub[i]->best()[0]->self().dominate(m_sub[j]->best()[0]->self())) {
						m_sub[j]->m_flag = true;
					}
					else {
						m_sub[i]->m_flag = true;
					}
				}
			}
		}
	}
	evaluation_tag DynDE::run_() {
		evaluation_tag tag = evaluation_tag::Normal;


		while (tag != evaluation_tag::Terminate) {
			for (size_t i = 0; i < m_M; ++i) {
				if (!m_sub[i]->m_flag)       tag = m_sub[i]->evolve();
		
				handle_evaluation_tag(tag);
				handle_evaluation_tag_all(tag);
				//HANDLE_RETURN_FLAG(rf)    //note by zhouli
			}

			if (tag == evaluation_tag::Terminate) break;
			//exclusion
			exclude();

			for (size_t i = 0; i < m_M; ++i) {
				//if (m_sub[i]->m_flag && (CAST_PROBLEM_DYN && !CAST_PROBLEM_DYN->predictChange(m_subPop[i]->m_popsize) || CAST_PROBLEM_DYN_ONEPEAK && !CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_subPop[i]->m_popsize))) 
				{
					//cout<<"Reinitial Evals: "<<i<<" "<<Global::msp_global->mp_problem->getEvaluations()<<endl;
					//m_subPop[i]->m_best.clear();
					m_sub[i]->initialize();

					//m_subPop[i]->printToScreen();
					//getchar();
				}
				if (terminating()) break;
				m_sub[i]->m_flag = false;
			}
			//measureMultiPop();
			//cout<<Global::gp_problem->getEvaluations()<<endl;
		}


		return tag;

	}
}
