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
A New Algorithm Using the Non-Dominated Tree to Improve Non-Dominated Sorting
January 2017Evolutionary Computation
DOI10.1162/EVCO_a_00204
*/

#ifndef ENS_NDT_H
#define ENS_NDT_H

#include "NDTree.h"

using namespace ENS_NDT;

namespace NS {
	int ENS_NDT_sort(const std::vector<std::vector<double>>& individuals, std::vector<int>& rank, std::pair<int, int>& measurement);
	class ENS_NDT {
	public:
		static int NondominatedSort(const std::vector<std::vector<double>>& P, int k, int& NumComp, std::vector<int>& rank);
	private:
		static int FrontIndexBinarySearch(std::vector<double>* s, std::vector<std::unique_ptr<NDTree>>& NDT, int& NumComp);
		static bool FitnessEquals(std::vector<double>& a, std::vector<double>& b, int k, int& NumComp);
	};
}

#endif // !ENS_NDT_H

