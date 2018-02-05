/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li 
* Email: changhe.lw@google.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

/*
Paper: J. Zhang and C. Arthur, ¡°JADE: Adaptive differential evolution with
optional external archive,¡± IEEE Trans. Evol. Comput., vol. 13, no. 5,
pp. 945¨C958, Oct. 2009.
*/
#ifndef JADE_H
#define JADE_H

#include "../DEindividual.h"
#include "../DEpopulation.h"
#include "../../../../core/measure/measure.h"

namespace OFEC {
	class JADEindi :public DEindividual<variable<real>, real> {
	public:
		using  DEindividual::select;
		template<typename ... Args>
		JADEindi(size_t no, Args&& ... args) : DEindividual(no, std::forward<Args>(args)...) {}
		evaluation_tag select(std::vector<solution_type>& archive) {
			m_impr = false;
			evaluation_tag tag = m_pu.evaluate();
			if (m_pu.dominate(*this)) {
				m_var = m_pu.get_variable();
				m_obj = m_pu.get_objective();
				m_constraint_value = m_pu.constraint_value();
				m_impr = true;
			}
			else {
				archive.push_back(*this);
			}
			return tag;
		}
	};

	class JADE :public DEpopulation<JADEindi>
	{
	public:
		//using JADEindi::solution_type;
		JADE(param_map &v);
		evaluation_tag run_();
	protected:
		evaluation_tag evolve();
		void selectTrial(int base);
		void updateF();
		void updateCR();
	protected:
		///////////////////////algorithm parameters/////////////////////////////
		double m_p;
		double m_c;
		int m_archiveFlag;
		std::vector<solution<variable<real>, real>> m_archive;
		std::vector<solution<variable<real>, real>*> m_candi;
		std::vector<double> m_pcentBest;
		std::vector<int> m_pcentBestIdx;
		std::vector<double> mv_F, mv_CR;
		///////////////////////////////////////////////////////////////////////////
	};
}
#endif // JADE_H
