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

//==================================================================
// This program was used in the following paper:
// H. Wang and X. Yao, ``Corner Sort for Pareto-based Many-Objective
// Optimization,'' IEEE Transactions on Cybernetics, 44(1):92-102, January
// 2014.
//
//==================================================================
#ifndef CONERSORT_H
#define CONERSORT_H

#include<malloc.h>
#include<stdlib.h>
#include <vector>
#include <chrono>

namespace NS{
	struct cs_node
	{
		unsigned int index;//index in POP
		cs_node *marknext;
		cs_node *markpre;
		cs_node *ranknext;
		cs_node *rankpre;
	};

	//==================================================================
	//unsigned int min_obj(double **POP,unsigned int obj_index,node **head,node **mh)
	//POP-population??obj_index-objective index??head-linked list head?? mh-the head of the mark linked list
	//Output:cur-the index of individual with the minimal obj_index-th objective
	//==================================================================
	unsigned int min_obj(const double **POP, unsigned int obj_index, cs_node **head, cs_node **mh, int *comp, const unsigned int m, int& NumComp);

	//==================================================================
	//void  cornerSort(double **POP,unsigned int m,unsigned int n,unsigned int *rank)
	//POP-population  m-No. of Objectives  n-size of POP  rank-the ranking result as output
	void  cornerSort(const double **POP, unsigned int m, unsigned int n, int *rank, int* comp, int& NumComp);
	void corner_sort(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement);
}
#endif