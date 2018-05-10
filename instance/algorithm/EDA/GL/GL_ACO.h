//#pragma once
/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 12 May 2016
// Last modified:
#include "MDL.h"
#include "../../ACO/Ant.h"
#include "../../Population.h"
class MDLACO: public MDL<CodeVInt, Ant,Population<CodeVInt, Ant>>{
public:
	MDLACO(ParamMap &v);
	ReturnFlag run_();
protected:
	ReturnFlag initialize_AS();
	ReturnFlag initialize_ACS();
	ReturnFlag initialize_MMAS();
	void local_updatePheromeno(vector<vector<double>> &phero, double t, int ant_loc, bool isLastEdge = false);
	double getLenOfNN();   //get the length of tour which is created based on nearest neighbor heuristic
	void statisticDiffsAndImp();
	
	void resetAntsInfo();
private:
	int m_num;
	static vector<set<int>> mv_candidate;
};