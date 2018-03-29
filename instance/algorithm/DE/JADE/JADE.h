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
// updated Mar 28, 2018 by Li Zhou

/*
Paper: J. Zhang and C. Arthur, ¡°JADE: Adaptive differential evolution with
optional external archive,¡± IEEE Trans. Evol. Comput., vol. 13, no. 5,
pp. 945¨C958, Oct. 2009.
*/
#ifndef JADE_H
#define JADE_H

#include "../DE_individual.h"
#include "../DE_population.h"
#include "../../../../core/measure/measure.h"

namespace OFEC {
	namespace DE {
		class JADE_individual :public individual<variable<real>, real> {
		public:
			using  individual::select;
			template<typename ... Args>
			JADE_individual(size_t no, Args&& ... args) : individual(no, std::forward<Args>(args)...) {}
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

		class JADE :public population<JADE_individual>
		{
		public:
			//using JADE_individual::solution_type;
			JADE(param_map &v);
			evaluation_tag run_();
		protected:
			evaluation_tag evolve();
			void select_trial(int base);
			void update_F();
			void update_CR();
		protected:
			///////////////////////algorithm parameters/////////////////////////////
			double m_p;
			double m_c;
			int m_archive_flag;
			std::vector<solution<variable<real>, real>> m_archive;
			std::vector<solution<variable<real>, real>*> m_candi;
			std::vector<double> m_pcent_best;
			std::vector<int> m_pcent_best_index;
			std::vector<double> mv_F, mv_CR;
			///////////////////////////////////////////////////////////////////////////
		};
	}
	using JADE = DE::JADE;
}
#endif // JADE_H
