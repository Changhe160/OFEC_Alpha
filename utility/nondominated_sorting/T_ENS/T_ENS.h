/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Junchen Wang
* Email: changhe.lw@gmail.com, wangjunchen@cug.edu.cn
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 29 Mar 2018 by Junchen Wang
// Last modified:

/*
Zhang, X., Tian, Y., Cheng, R., & Jin, Y. (2016).
A decision variable clustering-based evolutionary algorithm for large-scale many-objective optimization.
IEEE Transactions on Evolutionary Computation.
*/
#ifndef T_ENS_H
#define T_ENS_H

#include "../../functional.h"
#include <chrono>

namespace NS {

	// return FrontNo
	void T_ENS(const std::vector<std::vector<double>>& Population, std::vector<int>& te_rank, std::pair<int, int>& measurement, int nSort = -1); 
}

#endif // !T_ENS_H

