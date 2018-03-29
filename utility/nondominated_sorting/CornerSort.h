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

namespace NDS{
	struct node
	{
		unsigned int index;//index in POP
		node *marknext;
		node *markpre;
		node *ranknext;
		node *rankpre;
	};

	//==================================================================
	//unsigned int min_obj(double **POP,unsigned int obj_index,node **head,node **mh)
	//POP-population??obj_index-objective index??head-linked list head?? mh-the head of the mark linked list
	//Output:cur-the index of individual with the minimal obj_index-th objective
	//==================================================================
	unsigned int min_obj(const double **POP, unsigned int obj_index, node **head, node **mh, int *comp, const unsigned int m, int& NumComp);

	//==================================================================
	//void  cornerSort(double **POP,unsigned int m,unsigned int n,unsigned int *rank)
	//POP-population  m-No. of Objectives  n-size of POP  rank-the ranking result as output
	void  cornerSort(const double **POP, unsigned int m, unsigned int n, int *rank, int* comp, int& NumComp);
	void CornerSort(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement);
}
#endif