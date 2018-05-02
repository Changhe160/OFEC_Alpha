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

/* This NS algorithm was introduced in:
Deb, K., Pratap, A., Agarwal, S., & Meyarivan, T. A. M. T. (2002).
A fast and elitist multiobjective genetic algorithm: NSGA-II.
IEEE transactions on evolutionary computation, 6(2), 182-197.
*/

#ifndef FAST_NONDOMINATED_SORT_H
#define FAST_NONDOMINATED_SORT_H

#include<map>
#include <list>
#include <vector>
#include "../../functional.h"
#include <chrono>

namespace NS {
	int fast_sort(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement);
#ifdef USING_CONCURRENT
	void ParallelCompare(int popsize, const std::vector<int>&& ks, const std::vector<std::vector<double>>& data, std::vector<int>& rank_, std::vector<int>& count, std::vector<std::vector<int>>& cset);
#endif // USING_CONCURRENT
}




#endif // !FAST_NONDOMINATED_SORT_H