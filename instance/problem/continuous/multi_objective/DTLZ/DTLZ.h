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
// Last modified:

/*
K. Deb, L. Thiele, M. Laumanns, and E. Zitzler, "Scalable test problems
for evolutionary multi - objective optimization, " in Evolutionary Multiobjective
Optimization, A.Abraham, L.Jain, and R.Goldberg, Eds.
London, U.K.: Springer - Verlag, 2005, pp. 105¨C145.
*/

#ifndef DTLZ_H
#define DTLZ_H

#include "../../../../../core/problem/continuous/continuous.h"

namespace OFEC {

	typedef std::vector<double> TObjVec;
	typedef std::vector<TObjVec> TFront;

	void generate_recursive(TFront *pf, TObjVec *pt, size_t num_objs, size_t left, size_t total, size_t element);
	void GenerateWeight(TFront *pf, size_t M, size_t p);
	void GeneratePF_OneLayer(std::ostream &os, const std::string &problem_name, int M, int p);
	void GeneratePF_TwoLayers(std::ostream &os, const std::string &problem_name, int M, int outside_p, int inside_p);

	class DTLZ : public continuous
	{
	protected:
		DTLZ(const std::string &name, size_t size_var, size_t size_obj);
		void generateAdLoadPF();
	};

}

#endif //DTLZ_H