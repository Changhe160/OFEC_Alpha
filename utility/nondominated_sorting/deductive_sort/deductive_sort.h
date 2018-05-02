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

#ifndef DEDUCTIVESORT_H
#define DEDUCTIVESORT_H

/*K.M.Clymont and E.Keedwell, ¡°Deductive sort and climbing sort :
new methods for non - dominated sorting, ¡± Evolutionary Computation,
vol. 20, no. 1, pp. 1¨C26, 2012.*/

#include <vector>
#include <chrono>

namespace NS {
	void deductive_sort(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement);
}

#endif // !DEDUCTIVESORT_H
