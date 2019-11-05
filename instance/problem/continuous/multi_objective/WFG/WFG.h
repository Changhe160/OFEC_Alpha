/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Xiaofang Wu
* Email: changhe.lw@google.com Or wuxiaofang@cug.edu.cn
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
/*********************************************************************************************************************
  S. Huband, L. Barone, L. While, and P. Hingston
  A scalable multi-objective test problem toolkit. Lecture Notes inComputer Science,2005, 3410:280-295.
  **********************************************************************************************************************/
// Created: 5 August 2019


#ifndef WFG_H
#define WFG_H

#include "../../../../../core/problem/continuous/continuous.h"
#include<cmath>
#include<algorithm>

namespace OFEC {
	class WFG : public continuous {
	protected:
		int m_k;				//position parameter(k) should be a multiple of M - 1, distance parameter(l) is N-k
		real m_D;				//distance parameter
		std::vector<real> m_S;	//shape parameter(scaling constants)(M)
		std::vector<real> m_A;	//degeneracy constants(M-1)
		std::vector<real> m_h;	//shape function
		
	public:		
		WFG(const std::string &name, size_t size_var, size_t size_obj);
		void initialize();
		void set_k(int k) { m_k = k; }
		void load_PF();
		void evaluate_objective(real *x, std::vector<real> &obj);
		std::vector<real> calculate_x(const std::vector<real> &t_p);
		std::vector<real> normalise(real *z);

		virtual std::vector<real> t1(const std::vector<real> &y) { auto t = y; return std::move(t); }
		virtual std::vector<real> t2(const std::vector<real> &y) { auto t = y; return std::move(t); }
		virtual std::vector<real> t3(const std::vector<real> &y) { auto t = y; return std::move(t); }
		virtual std::vector<real> t4(const std::vector<real> &y) { auto t = y; return std::move(t); }
		virtual std::vector<real> shape(const std::vector<real> &y) { auto t = y; return std::move(t); }

		real linear(const std::vector<real> &x, int m);	//The linear shape function. (m is indexed from 1.)
		real convex(const std::vector<real> &x, int m);	//The convex shape function. (m is indexed from 1.)		
		real concave(const std::vector<real> &x, int m);	//The concave shape function. (m is indexed from 1.)		
		real mixed(const std::vector<real> &x, int A, real alpha);	//The mixed convex/concave shape function		
		real disc(const std::vector<real> &x, int A, real alpha, real beta);	//The disconnected shape function
		real correct_to_01(const real &a, real epsilon = 1.0e-10);

		std::vector<real> subvector(const std::vector<real> &v, int head, int tail);

		real b_poly(real y, real alpha);
		real b_flat(real y, real A, real B, real C);
		real b_param(real y, real u, real A, real B, real C);
		real s_linear(real y, real A);
		real s_decept(real y, real A, real B, real C);
		real s_multi(real y, int A, real B, real C);
		real r_sum(const std::vector<real> &y, const std::vector<real> &w);
		real r_nonsep(const std::vector<real> &y, int A);

	};
}
#endif //MOEA_FBASE_H