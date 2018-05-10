/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* Operator adaptor for continous problems
*
*********************************************************************************/
#ifndef MDL_OPERATOR_CONTINUE
#define MDL_OPERATOR_CONTINUE
#include "adaptor.h"

#include <unordered_map>
namespace OFEC {
	template<typename Individual>
	class adaptor_cont :public adaptor<Individual> {
	public:
		adaptor_cont(int numDim, int popsize);
		~adaptor_cont() {}
		void updatePro(vector<unique_ptr<TypeIndi>> &pop, const vector<double>& weight, const vector<int> *index = nullptr);
		void updatePro(vector<TypeIndi> &pop, const vector<double>& weight, const vector<int> *index = nullptr);
		void createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, double alpha);
		ReturnFlag updateSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, int& impRadio, int ms);
		void printPro();
		void createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, vector<TypeIndi> &his, double alpha);
		void updateStep(vector<unique_ptr<TypeIndi>> &pop);
		double getInterDis();
		void createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, vector<double>& alpha);
		ReturnFlag evenDistri(vector<unique_ptr<TypeIndi>> &pop);
		double getIntraDis(vector<unique_ptr<TypeIndi>> &pop);
		ReturnFlag repel(vector<unique_ptr<TypeIndi>> &pop);
	protected:
		void accumlatePro();
	protected:
		int m_num;
		struct Infor {
			double val;
			vector<int> idx;
		};
		vector<vector<Infor> > mvv_pro;
		vector<vector<double> >mvv_accPro;
		struct Limit {
			pair<double, double>boundary;
			double step, range;
			vector<double> as;
		};
		vector<Limit> m_limit, m_initialRange;

		vector<vector<int>> m_pos;
	};

	template<typename TypeIndi>
	adaptor_cont<TypeIndi>::adaptor_cont(int numDim, int popsize) :mvv_pro(numDim, vector<Infor>(m_num)), mvv_accPro(numDim, vector<double>(m_num)), \
		m_num(100), m_limit(numDim), m_initialRange(numDim)
	{
		for (auto &i : m_limit) i.as.resize(popsize);
		for (auto &i : m_initialRange) {
			i.boundary.first = numeric_limits<double>::max();
			i.boundary.second = numeric_limits<double>::lowest();
		}

		m_pos.resize(popsize);
		for (auto &i : m_pos) i.resize(numDim);
	}

	template<typename TypeIndi>
	void adaptor_cont<TypeIndi>::updatePro(vector<unique_ptr<TypeIndi>> &pop, const vector<double>& weight, const vector<int> *index) {

		int popsize = pop.size();
		for (auto &i : mvv_pro) for (auto &j : i) {
			j.val = 0;
			j.idx.clear();
		}
		for (auto &i : m_limit) {
			i.boundary.first = numeric_limits<double>::max();
			i.boundary.second = numeric_limits<double>::lowest();
		}
		if (index == nullptr) {
			for (int i = 0; i < mvv_pro.size(); ++i) {
				for (int j = 0; j < popsize; ++j) {
					if (!pop[j]->isActive()) continue;
					if (m_limit[i].boundary.first > pop[j]->data().m_x[i])m_limit[i].boundary.first = pop[j]->data().m_x[i];
					if (m_limit[i].boundary.second < pop[j]->data().m_x[i])m_limit[i].boundary.second = pop[j]->data().m_x[i];
				}
			}
		}
		else {
			for (int i = 0; i < mvv_pro.size(); ++i) {
				for (auto j : (*index)) {
					if (!pop[j]->isActive()) continue;
					if (m_limit[i].boundary.first > pop[j]->data().m_x[i])m_limit[i].boundary.first = pop[j]->data().m_x[i];
					if (m_limit[i].boundary.second < pop[j]->data().m_x[i])m_limit[i].boundary.second = pop[j]->data().m_x[i];
				}
			}
		}

		if (index == nullptr) {
			for (int i = 0; i < mvv_pro.size(); ++i) {
				m_limit[i].range = m_limit[i].boundary.second - m_limit[i].boundary.first;
				if (m_limit[i].range > m_initialRange[i].range) m_initialRange[i].range = m_limit[i].range;
				m_limit[i].step = m_limit[i].range / m_num;
				if (m_limit[i].step > 0) {
					for (int j = 0; j < popsize; ++j) {
						if (!pop[j]->isActive()) continue;
						int reg_idx = (pop[j]->data().m_x[i] - m_limit[i].boundary.first) / m_limit[i].step;
						if (reg_idx == m_num) reg_idx--;
						mvv_pro[i][reg_idx].val += weight[j];
						mvv_pro[i][reg_idx].idx.push_back(j);

						m_pos[j][i] = reg_idx;
					}
				}
				else {
					for (int j = 0; j < popsize; ++j) {
						if (!pop[j]->isActive()) continue;
						mvv_pro[i][0].val += weight[j];
						mvv_pro[i][0].idx.push_back(j);

						m_pos[j][i] = 0;
					}
				}
			}
		}
		else {
			for (int i = 0; i < mvv_pro.size(); ++i) {
				m_limit[i].range = m_limit[i].boundary.second - m_limit[i].boundary.first;
				if (m_limit[i].range > m_initialRange[i].range) m_initialRange[i].range = m_limit[i].range;
				m_limit[i].step = m_limit[i].range / m_num;
				if (m_limit[i].step > 0) {
					for (auto j : (*index)) {
						if (!pop[j]->isActive()) continue;
						int reg_idx = (pop[j]->data().m_x[i] - m_limit[i].boundary.first) / m_limit[i].step;
						if (reg_idx == m_num) reg_idx--;
						mvv_pro[i][reg_idx].val += weight[j];
						mvv_pro[i][reg_idx].idx.push_back(j);

						m_pos[j][i] = reg_idx;
					}
				}
				else {
					for (auto j : (*index)) {
						if (!pop[j]->isActive()) continue;
						mvv_pro[i][0].val += weight[j];
						mvv_pro[i][0].idx.push_back(j);

						m_pos[j][i] = 0;
					}
				}
			}
		}
		accumlatePro();

	}

	template<typename TypeIndi>
	void adaptor_cont<TypeIndi>::updatePro(vector<TypeIndi> &pop, const vector<double>& weight, const vector<int> *index) {
		int popsize = pop.size();
		for (auto &i : mvv_pro) for (auto &j : i) {
			j.val = 0;
			j.idx.clear();
		}

		for (auto &i : m_limit) {
			i.boundary.first = numeric_limits<double>::max();
			i.boundary.second = numeric_limits<double>::lowest();
		}
		if (index == nullptr) {
			for (int i = 0; i < mvv_pro.size(); ++i) {
				for (int j = 0; j < popsize; ++j) {
					if (m_limit[i].boundary.first > pop[j].data().m_x[i])m_limit[i].boundary.first = pop[j].data().m_x[i];
					if (m_limit[i].boundary.second < pop[j].data().m_x[i])m_limit[i].boundary.second = pop[j].data().m_x[i];
				}
			}
		}
		else {
			for (int i = 0; i < mvv_pro.size(); ++i) {
				for (auto j : (*index)) {
					if (m_limit[i].boundary.first > pop[j].data().m_x[i])m_limit[i].boundary.first = pop[j].data().m_x[i];
					if (m_limit[i].boundary.second < pop[j].data().m_x[i])m_limit[i].boundary.second = pop[j].data().m_x[i];
				}
			}
		}

		if (index == nullptr) {
			for (int i = 0; i < mvv_pro.size(); ++i) {
				m_limit[i].range = m_limit[i].boundary.second - m_limit[i].boundary.first;
				if (m_limit[i].range > m_initialRange[i].range) m_initialRange[i].range = m_limit[i].range;
				m_limit[i].step = m_limit[i].range / m_num;
				if (m_limit[i].step > 0) {
					for (int j = 0; j < popsize; ++j) {
						int reg_idx = (pop[j].data().m_x[i] - m_limit[i].boundary.first) / m_limit[i].step;
						if (reg_idx == m_num) reg_idx--;
						mvv_pro[i][reg_idx].val += weight[j];
						mvv_pro[i][reg_idx].idx.push_back(j);

						m_pos[j][i] = reg_idx;
					}
				}
				else {
					for (int j = 0; j < popsize; ++j) {
						mvv_pro[i][0].val += weight[j];
						mvv_pro[i][0].idx.push_back(j);

						m_pos[j][i] = 0;
					}
				}
			}
		}
		else {
			for (int i = 0; i < mvv_pro.size(); ++i) {
				m_limit[i].range = m_limit[i].boundary.second - m_limit[i].boundary.first;
				if (m_limit[i].range > m_initialRange[i].range) m_initialRange[i].range = m_limit[i].range;
				m_limit[i].step = m_limit[i].range / m_num;
				if (m_limit[i].step > 0) {
					for (auto j : (*index)) {
						int reg_idx = (pop[j].data().m_x[i] - m_limit[i].boundary.first) / m_limit[i].step;
						if (reg_idx == m_num) reg_idx--;
						mvv_pro[i][reg_idx].val += weight[j];
						mvv_pro[i][reg_idx].idx.push_back(j);
						m_pos[j][i] = reg_idx;
					}
				}
				else {
					for (auto j : (*index)) {
						mvv_pro[i][0].val += weight[j];
						mvv_pro[i][0].idx.push_back(j);
						m_pos[j][i] = 0;
					}
				}
			}
		}

		accumlatePro();

	}

	template<typename TypeIndi>
	void adaptor_cont<TypeIndi>::createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, double alpha) {
		int popsize = pop.size();
		int numDim = mvv_pro.size();
		int mode = Global::msp_global->mp_problem->getOptType();
		double x;
		BoundaryCont & range = CAST_PROBLEM_CONT->getSearchRange();

		for (int j = 0; j < popsize; j++) {
			if (!pop[j]->isActive()) continue;
			for (int i = 0; i < numDim; i++) {
				double p = Global::msp_global->mp_uniformAlg->Next();
				double center = 0, var = 0;
				if (alpha < 0)		//alpha = exp((m_limit[i].range / m_initialRange[i].range - 1));
					alpha = Global::msp_global->mp_uniformAlg->Next();
				if (p <= alpha) {
					center = pop[j]->data().m_x[i];
					int idex = (center - m_limit[i].boundary.first) / m_limit[i].step;
					if (idex < m_num&& idex >= 0) {
						for (auto k = 0; k < mvv_pro[i][idex].idx.size(); ++k) {
							var += m_limit[i].as[mvv_pro[i][idex].idx[k]];
						}
						var /= mvv_pro[i][idex].idx.size();
						if (mvv_pro[i][idex].idx.size() == 0) {
							cout << "error" << endl;
						}
					}
					else var = m_limit[i].step;
				}
				else {

					vector<double> pro(m_num), acc_pro(m_num);
					for (int k = 0; k < m_num; ++k) {
						pro[k] = mvv_pro[i][k].val*exp(-fabs(m_pos[j][i] - k) / m_num);
						acc_pro[k] = pro[k];
						if (k > 0) acc_pro[k] += acc_pro[k - 1];
					}

					p = Global::msp_global->mp_uniformAlg->Next()*acc_pro[m_num - 1];
					int idex = lower_bound(acc_pro.begin(), acc_pro.end(), p) - acc_pro.begin();


					/*p = Global::msp_global->mp_uniformAlg->Next()*mvv_accPro[i][m_num - 1];
					int idex = lower_bound(mvv_accPro[i].begin(), mvv_accPro[i].end(), p) - mvv_accPro[i].begin();

					if (mvv_pro[i][idex].idx.size() == 0) {
						cout << "error" << endl;
					}*/

					for (auto k = 0; k < mvv_pro[i][idex].idx.size(); ++k) {
						center += pop[mvv_pro[i][idex].idx[k]]->data().m_x[i];
					}
					center /= mvv_pro[i][idex].idx.size();

					for (auto k = 0; k < mvv_pro[i][idex].idx.size(); ++k) {
						var += m_limit[i].as[mvv_pro[i][idex].idx[k]];
					}
					var /= mvv_pro[i][idex].idx.size();


					/*for (auto k = 0; k < mvv_pro[i][idex].idx.size(); ++k) {
						var += (pop[mvv_pro[i][idex].idx[k]]->data().m_x[i] - center)*(pop[mvv_pro[i][idex].idx[k]]->data().m_x[i] - center);
					}
					var = sqrt(var / mvv_pro[i][idex].idx.size());*/

				}
				if (center < range[i].m_lower) center = range[i].m_lower;
				else if (center > range[i].m_upper) center = range[i].m_upper;

				x = center + var* Global::msp_global->mp_cauchyAlg->Next();
				//x = center + var* Global::msp_global->mp_normalAlg->Next();

				if (x < range[i].m_lower) {
					x = (center + range[i].m_lower) / 2;
				}
				else if (x > range[i].m_upper) {
					x = (center + range[i].m_upper) / 2;
				}
				curPop[j].data().m_x[i] = x;
			}
		}
	}

	template<typename TypeIndi>
	void adaptor_cont<TypeIndi>::createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, vector<TypeIndi> &his, double alpha) {
		int popsize = pop.size();
		int numDim = mvv_pro.size();
		int mode = Global::msp_global->mp_problem->getOptType();
		double x;
		BoundaryCont & range = CAST_PROBLEM_CONT->getSearchRange();

		for (int j = 0; j < popsize; j++) {
			for (int i = 0; i < numDim; i++) {
				double p = Global::msp_global->mp_uniformAlg->Next();
				double center = 0;
				if (alpha < 0)
					alpha = exp((m_limit[i].range / m_initialRange[i].range - 1));
				if (p <= alpha) {
					center = pop[j]->data().m_x[i];
				}
				else {
					p = Global::msp_global->mp_uniformAlg->Next()*mvv_accPro[i][m_num - 1];
					int idex = lower_bound(mvv_accPro[i].begin(), mvv_accPro[i].end(), p) - mvv_accPro[i].begin();

					for (auto k = 0; k < mvv_pro[i][idex].idx.size(); ++k) {
						center += his[mvv_pro[i][idex].idx[k]].data().m_x[i];
					}
					center /= mvv_pro[i][idex].idx.size();
				}
				if (center < range[i].m_lower) center = range[i].m_lower;
				else if (center > range[i].m_upper) center = range[i].m_upper;

				x = center + m_limit[i].step * Global::msp_global->mp_cauchyAlg->Next();
				//x = center + m_limit[i].step * Global::msp_global->mp_normalAlg->Next();
				//x =  Global::msp_global->mp_normalAlg->NextNonStand(center, m_limit[i].step*m_limit[i].step);
				if (x < range[i].m_lower) {
					x = (center + range[i].m_lower) / 2;
				}
				else if (x > range[i].m_upper) {
					x = (center + range[i].m_upper) / 2;
				}
				curPop[j].data().m_x[i] = x;
			}
		}
	}

	template<typename TypeIndi>
	void adaptor_cont<TypeIndi>::accumlatePro() {

		for (auto i = 0; i < mvv_accPro.size(); ++i) {
			for (auto j = 0; j < mvv_accPro[i].size(); ++j) {
				mvv_accPro[i][j] = mvv_pro[i][j].val;
				if (j > 0) mvv_accPro[i][j] += mvv_accPro[i][j - 1];
			}
		}
	}

	template<typename TypeIndi>
	ReturnFlag adaptor_cont<TypeIndi>::updateSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, int& impRadio, int ms) {

		ReturnFlag rf = Return_Normal;
		int popsize = pop.size();
		impRadio = 0;
		for (int i = 0; i < popsize; i++)
		{
			if (!pop[i]->isActive()) continue;
			rf = curPop[i].evaluate();
			if (*pop[i] < curPop[i]) {
				pop[i]->data() = curPop[i].data();
				pop[i]->setFlag(true);
				impRadio++;
			}
			if (rf == Return_Terminate) break;
		}

		return rf;
	}

	template<typename TypeIndi>
	void adaptor_cont<TypeIndi>::printPro() {
		double mean1 = 0, mean2 = 0;
		for (auto i = 0; i < m_limit.size(); ++i) {
			mean1 += m_limit[i].range / m_initialRange[i].range;
			//mean2 += m_limit[i].range;
			//cout <<i<<" "<< m_limit[i].boundary.first << " " << m_limit[i].boundary.second << endl;

		}
		cout << setw(15) << mean1 / m_limit.size() << endl;
	}

	template<typename TypeIndi>
	double adaptor_cont<TypeIndi>::getInterDis() {
		double mean1 = 0;
		for (auto i = 0; i < m_limit.size(); ++i) {
			mean1 += m_limit[i].range / m_initialRange[i].range;
		}
		return mean1 / m_limit.size();
	}

	template<typename TypeIndi>
	double adaptor_cont<TypeIndi>::getIntraDis(vector<unique_ptr<TypeIndi>> &pop) {
		double mean = 0;
		int cnt = 0;
		for (int i = 0; i < mvv_pro.size(); ++i) {
			vector<double> x(pop.size());
			for (int j = 0; j < pop.size(); ++j) {
				x[j] = pop[j]->data().m_x[i];
			}
			std::sort(x.begin(), x.end());
			double m = 0;
			for (int j = 0; j < x.size(); ++j) {
				if (j == 0) {
					m += fabs(x[j] - x[j + 1]);
				}
				else if (j == x.size() - 1) {
					m += fabs(x[j] - x[j - 1]);
				}
				else {
					m += fabs(x[j] - x[j + 1]) < fabs(x[j] - x[j - 1]) ? fabs(x[j] - x[j + 1]) : fabs(x[j] - x[j - 1]);
				}
				mean += m / x.size();
			}
		}
		return mean / mvv_pro.size();
	}

	template<typename TypeIndi>
	void adaptor_cont<TypeIndi>::updateStep(vector<unique_ptr<TypeIndi>> &pop) {
		int popsize = pop.size();
		int numDim = mvv_pro.size();
		vector<double*> loc(popsize);
		vector<int> idx(popsize);
		for (int d = 0; d < numDim; ++d) {
			for (int i = 0; i < popsize; i++)
			{
				loc[i] = &pop[i]->data().m_x[d];
			}
			gQuickSort(loc, popsize, idx);
			double lb, ub;
			CAST_PROBLEM_CONT->getSearchRange(lb, ub, d);
			for (int i = 0; i < popsize; i++)
			{
				if (i == 0) { //lower boundary
					//m_limit[d].as[idx[i]] = *loc[idx[i + 1]] - *loc[idx[i]];
					m_limit[d].as[idx[i]] = (*loc[idx[i + 1]] - lb) / 2;
				}
				else if (i == popsize - 1) { //upper boundary
					//m_limit[d].as[idx[i]] = *loc[idx[i]]- *loc[idx[i - 1]];
					m_limit[d].as[idx[i]] = (ub - *loc[idx[i - 1]]) / 2;
				}
				else {
					double d1 = *loc[idx[i + 1]] - *loc[idx[i]], d2 = *loc[idx[i]] - *loc[idx[i - 1]];
					m_limit[d].as[idx[i]] = (d1 + d2) / 2;//d1 < d2 ? d1 : d2;
				}
			}
		}

	}

	template<typename TypeIndi>
	void adaptor_cont<TypeIndi>::createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, vector<double>& alpha) {
		int popsize = pop.size();
		int numDim = mvv_pro.size();
		int mode = Global::msp_global->mp_problem->getOptType();
		double x;
		BoundaryCont & range = CAST_PROBLEM_CONT->getSearchRange();

		for (int j = 0; j < popsize; j++) {
			if (!pop[j]->isActive()) continue;
			for (int i = 0; i < numDim; i++) {
				double p = Global::msp_global->mp_uniformAlg->Next();
				double center = 0, var = 0;
				if (alpha[j] < 0)		alpha[j] = exp((m_limit[i].range / m_initialRange[i].range - 1));
				if (p <= alpha[j]) {
					center = pop[j]->data().m_x[i];
					int idex = (center - m_limit[i].boundary.first) / m_limit[i].step;
					if (idex < m_num&& idex >= 0) {
						for (auto k = 0; k < mvv_pro[i][idex].idx.size(); ++k) {
							var += m_limit[i].as[mvv_pro[i][idex].idx[k]];
						}
						var /= mvv_pro[i][idex].idx.size();
					}
					else var = m_limit[i].step;
				}
				else {
					p = Global::msp_global->mp_uniformAlg->Next()*mvv_accPro[i][m_num - 1];
					int idex = lower_bound(mvv_accPro[i].begin(), mvv_accPro[i].end(), p) - mvv_accPro[i].begin();

					for (auto k = 0; k < mvv_pro[i][idex].idx.size(); ++k) {
						center += pop[mvv_pro[i][idex].idx[k]]->data().m_x[i];
					}
					center /= mvv_pro[i][idex].idx.size();

					for (auto k = 0; k < mvv_pro[i][idex].idx.size(); ++k) {
						var += m_limit[i].as[mvv_pro[i][idex].idx[k]];
					}
					var /= mvv_pro[i][idex].idx.size();
				}
				if (center < range[i].m_lower) center = range[i].m_lower;
				else if (center > range[i].m_upper) center = range[i].m_upper;

				//x = center + var* Global::msp_global->mp_cauchyAlg->Next();
				x = center + var* Global::msp_global->mp_normalAlg->Next();

				if (x < range[i].m_lower) {
					x = (center + range[i].m_lower) / 2;
				}
				else if (x > range[i].m_upper) {
					x = (center + range[i].m_upper) / 2;
				}
				curPop[j].data().m_x[i] = x;
			}
		}
	}

	template<typename TypeIndi>
	ReturnFlag adaptor_cont<TypeIndi>::evenDistri(vector<unique_ptr<TypeIndi>> &pop) {
		int popsize = pop.size();
		int numDim = mvv_pro.size();
		vector<bool> flag(popsize);
		list<vector<pair<double, double>>> box;
		list<list<int>> distri;
		int cnt = 0;
		while (cnt < popsize) {
			int idx = -1;
			while (flag[++idx]);
			vector<pair<double, double>> b(numDim);
			for (int i = 0; i < numDim; ++i) {
				b[i].first = m_limit[i].boundary.first + floor((pop[idx]->data().m_x[i] - m_limit[i].boundary.first) / m_limit[i].step)*m_limit[i].step;
				b[i].second = b[i].first + m_limit[i].step;
			}
			list<int> indis;
			for (int i = idx; i < popsize; ++i) {
				if (!flag[i]) {
					bool inside = true;
					if (i != idx) {
						for (int d = 0; d < numDim; ++d) {
							if (b[d].first > pop[i]->data().m_x[d] || pop[i]->data().m_x[d] >= b[d].second) {
								inside = false;
								break;
							}
						}
					}
					if (inside) {
						auto it = indis.begin();
						while (it != indis.end() && *pop[i] < *pop[*it]) it++;
						indis.insert(it, i);
						cnt++;
						flag[i] = true;
					}
				}
			}
			distri.push_back(move(indis));
			box.push_back(move(b));
		}

		int avg = ceil(popsize*1.0 / box.size());

		list<int> crowd;
		cout << distri.size() << " " << avg << endl;
		//collect crowded individuals
		for (auto it = distri.begin(); it != distri.end(); ++it) {
			if (it->size() > avg) {
				cnt = -1;
				auto i = it->begin();
				while (++cnt < avg) i++;
				crowd.splice(crowd.end(), *it, i, it->end());
			}
		}
		// redistribute crowded individuals
		ReturnFlag rf = Return_Normal;
		while (crowd.size() > 0) {
			auto idx = box.begin();
			for (auto it = distri.begin(); it != distri.end(); ++it, ++idx) {
				if (crowd.size() == 0) break;
				if (it->size() < avg) {
					for (int d = 0; d < numDim; ++d) {
						double l1 = pop[it->front()]->data().m_x[d] - idx->at(d).first, l2 = idx->at(d).second - pop[it->front()]->data().m_x[d];
						l1 = l1 < l2 ? l1 : l2;

						double x = pop[it->front()]->data().m_x[d] + l1*Global::msp_global->mp_normalAlg->Next();
						if (x < idx->at(d).first) x = idx->at(d).first;
						else if (x > idx->at(d).second) x = idx->at(d).second;

						pop[crowd.front()]->data().m_x[d] = x;
					}
					rf = pop[crowd.front()]->evaluate();
					it->splice(it->end(), crowd, crowd.begin());
					if (rf != Return_Normal) return rf;
				}
			}
		}
		return rf;
	}

	template<typename TypeIndi>
	ReturnFlag adaptor_cont<TypeIndi>::repel(vector<unique_ptr<TypeIndi>> &pop) {
		int numDim = mvv_pro.size();

		std::map<vector<int>, vector<int>> cnt;
		for (auto i = 0; i < pop.size(); ++i) {
			cnt[m_pos[i]].push_back(i);
		}
		int mean = pop.size() / cnt.size();

		vector<int> redundant;
		vector<std::map<vector<int>, vector<int>>::iterator> insert;

		vector<bool> label(cnt.size(), false);
		int k = 0;
		for (auto i = cnt.begin(); i != cnt.end(); ++i, ++k) {
			while (i->second.size() > mean + 1) {
				//label[k] = true;
				int idx = 0;
				for (auto j = 1; j < i->second.size(); ++j) {
					if (*pop[i->second[idx]] > *pop[i->second[j]]) {
						idx = j;
					}
				}
				redundant.push_back(i->second[idx]);
				i->second.erase(i->second.begin() + idx);
			}
		}

		for (auto i = cnt.begin(); i != cnt.end(); ++i, ++k) {
			if (i->second.size() > 1) {
				double dis = 0;
				for (auto j = 1; j < i->second.size(); ++j) {
					dis += pop[i->second[j]]->getDistance(pop[i->second[0]]->data());
				}
				if (dis / i->second.size() < 1.e-5) {
					for (auto j = 0; j < i->second.size(); ++j) {
						pop[i->second[j]]->setActive(false);
						//cout << "sleep" << endl;
					}
				}
			}
		}


		ReturnFlag rf = Return_Normal;

		for (auto &r : redundant) {
			int max_dis = numeric_limits<int>::max();
			decltype (cnt.begin()) it;
			k = 0;
			int idx = 0;
			for (auto i = cnt.begin(); i != cnt.end(); ++i, ++k) {
				if (label[k]) continue;
				int dis = 0;
				for (int d = 0; d < m_pos[r].size(); ++d) {
					dis += fabs(m_pos[r][d] - i->first[d]);
				}
				if (dis < max_dis) {
					max_dis = dis;
					it = i;
					idx = k;
				}
			}
			insert.push_back(it);
			label[idx] = true;
			if (label.end() == find_if(label.begin(), label.end(), [](bool i) {return i == false; })) {
				//for (auto&i : label) i = false;
				break;
			}
		}

		auto itr = redundant.begin();
		for (auto i = insert.begin(); i != insert.end(); ++i, ++itr) {
			int best;
			int idx = 0;
			for (auto j = 1; j < (*i)->second.size(); ++j) {
				if (*pop[(*i)->second[idx]] < *pop[(*i)->second[j]]) {
					idx = j;
				}
			}
			best = (*i)->second[idx];

			//initialize an indi inside in the region where i->fist is
			for (int d = 0; d < numDim; ++d) {
				double lb = m_limit[d].boundary.first + m_limit[d].step*(*i)->first[d], rb = m_limit[d].boundary.first + m_limit[d].step*(1 + (*i)->first[d]);
				double l = pop[best]->data().m_x[d] - lb;
				double r = rb - pop[best]->data().m_x[d];
				double var = l < r ? l : r, x;
				if (var == 0) {
					var = m_initialRange[d].range / pop.size();
				}
				x = pop[best]->data().m_x[d] + var*Global::msp_global->mp_normalAlg->Next();
				if (x < lb) x = (lb + pop[best]->data().m_x[d]) / 2;
				else if (x > rb) x = (rb + pop[best]->data().m_x[d]) / 2;
				pop[*itr]->data().m_x[d] = x;
			}
			(*i)->second.push_back(*itr);
			pop[*itr]->setActive(true);
			pop[*itr]->setFlag(true);

			rf = pop[*itr]->evaluate();
			if (rf != Return_Normal) return rf;
		}

		while (itr != redundant.end()) {
			pop[*itr]->setActive(true);
			pop[*itr]->setFlag(true);
			rf = pop[*itr]->initialize();
			if (rf != Return_Normal) return rf;
			++itr;
		}

		return rf;

	}
}

#endif // !MDL_OPERATOR_CONTINUE
