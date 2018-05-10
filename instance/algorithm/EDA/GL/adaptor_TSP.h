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


#ifndef MDLOPERATOR_TSP_H
#define MDLOPERATOR_TSP_H

#include "MDLOperator.h"

template<typename TypeIndi>
class MDLOperator_TSP :public MDLOperator<TypeIndi> {
public:
	MDLOperator_TSP(int numDim);
	~MDLOperator_TSP() {}
	void updatePro( vector<unique_ptr<TypeIndi>> &pop, const vector<double>& weight, const vector<int> *index = nullptr);
	void updatePro( vector<TypeIndi> &pop, const vector<double>& weight, const vector<int> *index = nullptr);
	void createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, double alpha);
	ReturnFlag updateSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, int& impRadio, int ms);
	void printPro();
	void printMatrix(int iter);
protected:
	vector<vector<double> > mvv_pro;

	vector<set<int>> m_range;
	vector<int> m_initialRange;
	pair<double, double> m_limit;

};


template<typename TypeIndi>
MDLOperator_TSP<TypeIndi>::MDLOperator_TSP(int numDim) {
	mvv_pro.resize(numDim);
	for (int i = 0; i < numDim; i++)
		mvv_pro[i].resize(numDim);

	m_range.resize(numDim);
	m_initialRange.resize(numDim,0);
	m_limit=make_pair(0.5, 0.9);
}


template<typename TypeIndi>
void MDLOperator_TSP<TypeIndi>::updatePro( vector<unique_ptr<TypeIndi>> &pop, const vector<double>& weight, const vector<int> *index) {
	int numDim = mvv_pro.size();
	int popsize = pop.size();
	for (int i = 0; i < numDim; ++i)
		for (int j = 0; j < numDim; ++j)
			mvv_pro[i][j] = 0;
	
	for (auto &i : m_range) i.clear();
	int e1, e2;
	if (index == nullptr) {
		for (int i = 0; i < popsize; ++i)
		{
			e1 = pop[i]->data().m_x[0];
			for (int j = 0; j < numDim; ++j)
			{
				e2 = pop[i]->data().m_x[(j + 1) % numDim];
				mvv_pro[e1][e2] += weight[i];
				mvv_pro[e2][e1] = mvv_pro[e1][e2];
				m_range[e1].insert(e2);
				m_range[e2].insert(e1);
				e1 = e2;
			}
		}
	}
	else {
		for (int i = 0; i < (*index).size(); ++i)
		{
			e1 = pop[(*index)[i]]->data().m_x[0];
			for (int j = 0; j < numDim; ++j)
			{
				e2 = pop[(*index)[i]]->data().m_x[(j + 1) % numDim];
				mvv_pro[e1][e2] += weight[(*index)[i]];
				mvv_pro[e2][e1] = mvv_pro[e1][e2];
				m_range[e1].insert(e2);
				m_range[e2].insert(e1);
				e1 = e2;
			}
		}
	}
	for (int j = 0; j < numDim; ++j) {
		if (m_range[j].size()>m_initialRange[j]) m_initialRange[j] = m_range[j].size();
	}
}

template<typename TypeIndi>
void MDLOperator_TSP<TypeIndi>::updatePro( vector<TypeIndi> &pop, const vector<double>& weight, const vector<int> *index) {
	int numDim = mvv_pro.size();
	int popsize = pop.size();
	for (int i = 0; i < numDim; ++i)
		for (int j = 0; j < numDim; ++j)
			mvv_pro[i][j] = 0;
	int e1, e2;
	if (index == nullptr) {
		for (int i = 0; i < popsize; ++i)
		{
			e1 = pop[i].data().m_x[0];
			for (int j = 0; j < numDim; ++j)
			{
				e2 = pop[i].data().m_x[(j + 1) % numDim];
				mvv_pro[e1][e2] += weight[i];
				mvv_pro[e2][e1] = mvv_pro[e1][e2];
				m_range[e1].insert(e2);
				m_range[e2].insert(e1);
				e1 = e2;
			}
		}
	}
	else {
		for (int i = 0; i < (*index).size(); ++i)
		{
			e1 = pop[(*index)[i]].data().m_x[0];
			for (int j = 0; j < numDim; ++j)
			{
				e2 = pop[(*index)[i]].data().m_x[(j + 1) % numDim];
				mvv_pro[e1][e2] += weight[(*index)[i]];
				mvv_pro[e2][e1] = mvv_pro[e1][e2];
				m_range[e1].insert(e2);
				m_range[e2].insert(e1);
				e1 = e2;
			}
		}
	}

	for (int j = 0; j < numDim; ++j) {
		if (m_range[j].size()>m_initialRange[j]) m_initialRange[j] = m_range[j].size();
	}
}


template<typename TypeIndi>
void MDLOperator_TSP<TypeIndi>::createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, double alpha) {
	int popsize = pop.size();
	int numDim = mvv_pro.size();

	for (int j = 0; j<popsize; j++) {
		curPop[j].initializeIteIndivl(pop[j]->self());		
		for (int i = 1; i<numDim; i++) {
			/*if (Global::g_arg[param_case] == 1) {
				const int &node = pop[j]->getCurrentNode();
				alpha = 0.1+0.85*2*(1/(1+exp(-4*(1-1.*m_range[node].size() / m_initialRange[node])))-0.5);
			}*/
			curPop[j].selectNext(*pop[j], mvv_pro, alpha);
		}
	}
}

template<typename TypeIndi>
ReturnFlag MDLOperator_TSP<TypeIndi>::updateSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, int& impRadio, int ms) {
	ReturnFlag rf = Return_Normal;
	int popsize = pop.size();
	int numDim = mvv_pro.size();
	impRadio = 0;

	for (int i = 0; i < popsize; i++)
		if (pop[i]->getFlag() == true)
			impRadio++;

	for (int i = 0; i < popsize; i++)
	{
		if (curPop[i].getFlag() == true)
		{
			if (pop[i]->getFlag() == true) 
				rf = pop[i]->evaluate();
			else
				rf = curPop[i].evaluate();
			if (rf == Return_Terminate) break;

			if (pop[i]->getFlag() == true && ms == MDLACO::c)
				rf = curPop[i].evaluate();
		}
		else {
			curPop[i].data().m_obj = pop[i]->data().m_obj;
		}
	}

	return rf;
}

template<typename TypeIndi>
void MDLOperator_TSP<TypeIndi>::printPro() {
	double mean1 = 0, mean2 = 0;
	for (auto i = 0; i < m_range.size(); ++i) {
		mean1 += (double)m_range[i].size() / m_initialRange[i];
		mean2 += m_range[i].size();
		
	}
	cout << setw(15) << mean1 / m_range.size() << " " << setw(15) << mean2 / m_range.size() << endl;
}
template<typename TypeIndi>
void MDLOperator_TSP<TypeIndi>::printMatrix(int iter) {
	vector<vector<double>> norm(mvv_pro);
	double min_ = std::numeric_limits<double>::max(), max_ = std::numeric_limits<double>::lowest();
	for (auto&i : norm) {
		for (auto&j : i) {
			if (j > max_) max_ = j;
			if (j < min_)min_ = j;
		}
	}

	for (auto&i : norm) {
		for (auto&j : i) {
			j = (j-min_) / (max_ - min_);
		}
	}
	
	ostringstream os;
	os <<"matrix_pop_"<< Global::g_arg[param_dataFile1]<<".txt";
	ofstream out(os.str());
	for (auto&i : norm) {
		for (auto&j : i) {
			out << j << " ";
		}
		out << endl;
	}
	out.close();
	vector<int> &opt = dynamic_cast<TravellingSalesman*>(Global::msp_global->mp_problem.get())->getGOpt()[0].data().m_x;

	for (auto&i : norm) {
		for (auto&j : i) {
			j =0;
		}
	}
	for (int i = 0; i < norm.size(); ++i) {
		int e1 = opt[i], e2 = opt[(i + 1) % norm.size()];
		norm[e1][e2] = norm[e2][e1] = 1;
	}
	os.str("");
	os.clear();

	os << "matrix_opt_" << Global::g_arg[param_dataFile1] << ".txt";
	out.open(os.str());
	for (auto&i : norm) {
		for (auto&j : i) {
			out << j << " ";
		}
		out << endl;
	}
	out.close();


}

#endif //MDLOPERATOR_TSP_H
