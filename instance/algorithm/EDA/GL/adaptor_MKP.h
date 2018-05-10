/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com  Or cugxiayong@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 20 Apr 2016
// Last modified:


#ifndef MDLOPERATOR_MKP_H
#define MDLOPERATOR_MKP_H

#include "MDLOperator.h"

template<typename TypeIndi>
class MDLOperator_MKP :public MDLOperator<TypeIndi> {
public:
	MDLOperator_MKP(int numDim);
	~MDLOperator_MKP() {}
	void updatePro( vector<unique_ptr<TypeIndi>> &pop, const vector<double>& weight, const vector<int> *index = nullptr);
	void updatePro( vector<TypeIndi> &pop, const vector<double>& weight, const vector<int> *index = nullptr);
	//void updatePro(const vector<vector<int>> &pop, const vector<double>& weight);
	void createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, double alpha);
	ReturnFlag updateSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, int& impRadio, int ms);
protected:
	double findMinWeight(const vector<double>& weight, const vector<int> *index = nullptr);
	void modifiyPro(const vector<double>& weight, const vector<int> *index = nullptr);
	double sumWeight(const vector<double>& weight, const vector<int> *index = nullptr);
	vector<double> mv_pro;
};


template<typename TypeIndi>
MDLOperator_MKP<TypeIndi>::MDLOperator_MKP(int numDim) {
	mv_pro.resize(numDim);
}


template<typename TypeIndi>
void MDLOperator_MKP<TypeIndi>::updatePro( vector<unique_ptr<TypeIndi>> &pop, const vector<double>& weight, const vector<int> *index) {
	int numDim = mv_pro.size();
	int popsize = pop.size();
	for (int i = 0; i < numDim; ++i)
			mv_pro[i] = 0;
	int e1;
	if (index == nullptr) {
		for (int i = 0; i < popsize; ++i)
		{
			for (int j = 0; j < numDim; ++j)
			{
				e1 = pop[i]->data().m_x[j];
				if (e1 == 1)
					mv_pro[j] += weight[i];
			}
		}
	}
	else {
		for (int i = 0; i < (*index).size(); ++i)
		{
			for (int j = 0; j < numDim; ++j)
			{
				e1 = pop[(*index)[i]]->data().m_x[j];
				if (e1 == 1)
					mv_pro[j] += weight[(*index)[i]];
			}
		}
	}

	modifiyPro(weight, index);
}

template<typename TypeIndi>
void MDLOperator_MKP<TypeIndi>::updatePro( vector<TypeIndi> &pop, const vector<double>& weight, const vector<int> *index) {
	int numDim = mv_pro.size();
	int popsize = pop.size();
	for (int i = 0; i < numDim; ++i)
			mv_pro[i] = 0;
	int e1;
	if (index == nullptr) {
		for (int i = 0; i < popsize; ++i)
		{
			for (int j = 0; j < numDim; ++j)
			{
				e1 = pop[i].data().m_x[j];
				if (e1 == 1)
					mv_pro[j] += weight[i];
			}
		}
	}
	else {
		for (int i = 0; i < (*index).size(); ++i)
		{
			for (int j = 0; j < numDim; ++j)
			{
				e1 = pop[(*index)[i]].data().m_x[j];
				if (e1 == 1)
					mv_pro[j] += weight[(*index)[i]];
			}
		}
	}

	modifiyPro(weight, index);
}

/*
template<typename TypeIndi>
void MDLOperator_MKP<TypeIndi>::updatePro(const vector<vector<int>> &pop, const vector<double>& weight) {
	int numDim = mv_pro.size();
	for (int i = 0; i < numDim; ++i)
			mv_pro[i] = 0;
	int e1;
	for (int i = 0; i < pop.size(); ++i)
	{
		for (int j = 0; j < numDim; ++j)
		{
			e1 = pop[i][j];
			if (e1 == 1)
				mv_pro[j] += weight[i];
		}
	}

	modifiyPro(weight);
}
*/
template<typename TypeIndi>
double MDLOperator_MKP<TypeIndi>::findMinWeight(const vector<double>& weight, const vector<int> *index) {
	double minWeight;
	if (index == nullptr) {
		minWeight = weight[0];
		for (int i = 1; i < weight.size(); i++) {
			if (minWeight > weight[i])
				minWeight = weight[i];
		}
	}
	else {
		minWeight = weight[(*index)[0]];
		for (int i = 1; i < (*index).size(); ++i) {
			if (minWeight > weight[(*index)[i]])
				minWeight = weight[(*index)[i]];
		}
	}
	return minWeight;
}

template<typename TypeIndi>
void MDLOperator_MKP<TypeIndi>::modifiyPro(const vector<double>& weight, const vector<int> *index) {
	int numDim = mv_pro.size();
	double minWeight = findMinWeight(weight, index);
	for (int i = 0; i < numDim; i++)
			if (mv_pro[i] == 0)
				mv_pro[i] = minWeight;
	double sum = sumWeight(weight, index);
	for (int i = 0; i < numDim; i++)
		mv_pro[i] /= sum;
}

template<typename TypeIndi>
double MDLOperator_MKP<TypeIndi>::sumWeight(const vector<double>& weight, const vector<int> *index) {
	double sum=0;
	if (index == nullptr) {
		for (int i = 0; i < weight.size(); i++) {
			sum += weight[i];
		}
	}
	else {
		for (int i = 0; i < (*index).size(); ++i) {
			sum += weight[(*index)[i]];
		}
	}
	return sum;
}

template<typename TypeIndi> 
void MDLOperator_MKP<TypeIndi>::createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, double alpha) {
	int popsize = pop.size();
	int numDim = mv_pro.size();
	for (int j = 0; j<popsize; j++) {
		for (int i = 1; i<numDim; i++) {
			curPop[j].selectNext(*pop[j], mv_pro, alpha);
		}
	}
}

template<typename TypeIndi>
ReturnFlag MDLOperator_MKP<TypeIndi>::updateSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, int& impRadio, int ms) {
	ReturnFlag rf = Return_Normal;
	int popsize = pop.size();
	int numDim = mv_pro.size();
	impRadio = 0;

	for (int i = 0; i < popsize; i++)
	{
		if (curPop[i].getFlag() == true)
		{
			rf = curPop[i].evaluate();
			if (curPop[i] > *pop[i])
			{
				*pop[i] = curPop[i];
				impRadio++;
			}
			if (rf == Return_Terminate) break;
		}
		else {
			curPop[i].data().m_obj = pop[i]->data().m_obj;
		}
	}

	return rf;
}

#endif //MDLOPERATOR_MKP_H
