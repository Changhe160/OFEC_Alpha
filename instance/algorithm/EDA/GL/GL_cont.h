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
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* GL of algorithms for continous problems
*
*********************************************************************************/


#ifndef MDL_CONTINUE
#define MDL_CONTINUE
#include "GL.h"
#include "../../../../core/algorithm/individual.h"
#include "../../../../core/algorithm/population.h"
#include "../../../../utility/kd-tree/kdtree_space.h"

namespace OFEC {
	namespace EDA {
		class GL_cont :public GL<population<individual<>>> {
		public:
			GL_cont(param_map &v);
			evaluation_tag run_();
			//GL_cont(vector<unique_ptr<Individual<CodeVReal>>>& indi);
			evaluation_tag evolve();
			
		protected:
			void initilize_curpop();
			bool removeConverged();
			void buildtree();
			void record();
			//void initializepar(vector<double>&beta, vector<double>&alpha);

			std::unique_ptr<KDTreeSpace::PartitioningKDTree<double>> m_tree;
			int m_numActive;
			//adaptation of beta and alpha
			//vector<double> m_accpb, m_hispb, m_accpa, m_hispa;
			//vector<pair<double, int>>m_pb, m_pa;
		};

	}
	using GL_cont = EDA::GL_cont;
}
#endif // !MDL_CONTINUE
