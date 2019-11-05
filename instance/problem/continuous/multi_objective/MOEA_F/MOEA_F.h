/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 31 December 2014
// Modified: 29 Mar 2018 by Junchen Wang (wangjunchen@cug.edu.cn)

/*********************************************************************************************************************
  H. Li and Q. Zhang
  Comparison Between NSGA-II and MOEA/D on a Set of Multiobjective Optimization Problems with Complicated Pareto Sets
  Technical Report CES-476, Department of Computer Science, University of Essex, 2007
**********************************************************************************************************************/

#ifndef MOEA_FBASE_H
#define MOEA_FBASE_H

#include "../../../../../core/problem/continuous/continuous.h"

namespace OFEC {
	class MOEA_FBase : public continuous {
	public:
		void initialize();
	protected:
		MOEA_FBase(const std::string &name, size_t size_var, size_t size_obj);
		int getDtype() const { return m_dtype; }
		int getPtype() const { return m_ptype; }
		int getLtype() const { return m_ltype; }
		// control the PF shape
		void alphafunction(real alpha[], const real *x, int dim, int type);
		// control the distance
		real betafunction(const std::vector<real> & x, int type);
		// control the PS shape of 2-d instances
		real psfunc2(const real &x, const real &t1, size_t dim, int type, int css);
		// control the PS shapes of 3-D instances
		real psfunc3(const real &x, const real &t1, const real &t2, int dim, int type);
		void calObjective(real *x, std::vector <real> &y_obj);
		void LoadPF();
		void evaluate_objective(real *x, std::vector<real> &obj) { calObjective(x, obj); }
		int m_dtype, m_ptype, m_ltype;
	};
}
#endif //MOEA_FBASE_H