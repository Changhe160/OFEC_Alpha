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
#include <limits>
#include <unordered_map>
#include <iomanip>
#include <list>
#include <vector>
#include <algorithm>
#include <utility>
#include "../../../../utility/nondominated_sorting/quick_sort.h"
#include "../../../../core/problem/continuous/continuous.h"

namespace OFEC {
	namespace EDA {
		template<typename Individual>
		class adaptor_cont :public adaptor<Individual> {
		public:
			adaptor_cont(int numDim, int popsize);
			~adaptor_cont() {}
			void update_probability(std::vector<std::unique_ptr<Individual>> &pop, const std::vector<double>& weight, const std::vector<int> *index = nullptr);
			void update_probability(std::vector<Individual> &pop, const std::vector<double>& weight, const std::vector<int> *index = nullptr);
			void create_solution(std::vector<std::unique_ptr<Individual>> &pop, std::vector<Individual> &curPop, double alpha);
			evaluation_tag update_solution(std::vector<std::unique_ptr<Individual>> &pop, std::vector<Individual> &curPop, int& impRadio, int ms);
			void print_infor();
			void create_solution(std::vector<std::unique_ptr<Individual>> &pop, std::vector<Individual> &curPop, std::vector<Individual> &his, double alpha);
			void update_step(std::vector<std::unique_ptr<Individual>> &pop);
			double get_inter_distance();
			void create_solution(std::vector<std::unique_ptr<Individual>> &pop, std::vector<Individual> &curPop, std::vector<double>& alpha);
			evaluation_tag evenDistri(std::vector<std::unique_ptr<Individual>> &pop);
			double get_intra_distance(std::vector<std::unique_ptr<Individual>> &pop);
			evaluation_tag repel(std::vector<std::unique_ptr<Individual>> &pop);
		protected:
			void accumlate_probability();
		protected:
			int m_num;
			struct Infor {
				double val;
				std::vector<int> idx;
			};
			std::vector<std::vector<Infor> > m_proba;
			std::vector<std::vector<double> >m_acc_proba;
			struct Limit {
				std::pair<double, double>boundary;
				double step, range;
				std::vector<double> as;
			};
			std::vector<Limit> m_limit, m_initialRange;

			std::vector<std::vector<int>> m_pos;
		};

		template<typename Individual>
		adaptor_cont<Individual>::adaptor_cont(int numDim, int popsize) :m_proba(numDim, std::vector<Infor>(m_num)), m_acc_proba(numDim, std::vector<double>(m_num)), \
			m_num(100), m_limit(numDim), m_initialRange(numDim)
		{
			for (auto &i : m_limit) i.as.resize(popsize);
			for (auto &i : m_initialRange) {
				i.boundary.first = std::numeric_limits<double>::max();
				i.boundary.second = std::numeric_limits<double>::lowest();
			}

			m_pos.resize(popsize);
			for (auto &i : m_pos) i.resize(numDim);
		}

		template<typename Individual>
		void adaptor_cont<Individual>::update_probability(std::vector<std::unique_ptr<Individual>> &pop, const std::vector<double>& weight, const std::vector<int> *index) {

			int popsize = pop.size();
			for (auto &i : m_proba) for (auto &j : i) {
				j.val = 0;
				j.idx.clear();
			}
			for (auto &i : m_limit) {
				i.boundary.first = std::numeric_limits<double>::max();
				i.boundary.second = std::numeric_limits<double>::lowest();
			}
			if (index == nullptr) {
				for (int i = 0; i < m_proba.size(); ++i) {
					for (int j = 0; j < popsize; ++j) {
						if (!pop[j]->is_active()) continue;
						if (m_limit[i].boundary.first > pop[j]->variable()[i])m_limit[i].boundary.first = pop[j]->variable()[i];
						if (m_limit[i].boundary.second < pop[j]->variable()[i])m_limit[i].boundary.second = pop[j]->variable()[i];
					}
				}
			}
			else {
				for (int i = 0; i < m_proba.size(); ++i) {
					for (auto j : (*index)) {
						if (!pop[j]->is_active()) continue;
						if (m_limit[i].boundary.first > pop[j]->variable()[i])m_limit[i].boundary.first = pop[j]->variable()[i];
						if (m_limit[i].boundary.second < pop[j]->variable()[i])m_limit[i].boundary.second = pop[j]->variable()[i];
					}
				}
			}

			if (index == nullptr) {
				for (int i = 0; i < m_proba.size(); ++i) {
					m_limit[i].range = m_limit[i].boundary.second - m_limit[i].boundary.first;
					if (m_limit[i].range > m_initialRange[i].range) m_initialRange[i].range = m_limit[i].range;
					m_limit[i].step = m_limit[i].range / m_num;
					if (m_limit[i].step > 0) {
						for (int j = 0; j < popsize; ++j) {
							if (!pop[j]->is_active()) continue;
							int reg_idx = (pop[j]->variable()[i] - m_limit[i].boundary.first) / m_limit[i].step;
							if (reg_idx == m_num) reg_idx--;
							m_proba[i][reg_idx].val += weight[j];
							m_proba[i][reg_idx].idx.push_back(j);

							m_pos[j][i] = reg_idx;
						}
					}
					else {
						for (int j = 0; j < popsize; ++j) {
							if (!pop[j]->is_active()) continue;
							m_proba[i][0].val += weight[j];
							m_proba[i][0].idx.push_back(j);

							m_pos[j][i] = 0;
						}
					}
				}
			}
			else {
				for (int i = 0; i < m_proba.size(); ++i) {
					m_limit[i].range = m_limit[i].boundary.second - m_limit[i].boundary.first;
					if (m_limit[i].range > m_initialRange[i].range) m_initialRange[i].range = m_limit[i].range;
					m_limit[i].step = m_limit[i].range / m_num;
					if (m_limit[i].step > 0) {
						for (auto j : (*index)) {
							if (!pop[j]->is_active()) continue;
							int reg_idx = (pop[j]->variable()[i] - m_limit[i].boundary.first) / m_limit[i].step;
							if (reg_idx == m_num) reg_idx--;
							m_proba[i][reg_idx].val += weight[j];
							m_proba[i][reg_idx].idx.push_back(j);

							m_pos[j][i] = reg_idx;
						}
					}
					else {
						for (auto j : (*index)) {
							if (!pop[j]->is_active()) continue;
							m_proba[i][0].val += weight[j];
							m_proba[i][0].idx.push_back(j);

							m_pos[j][i] = 0;
						}
					}
				}
			}
			accumlate_probability();

		}

		template<typename Individual>
		void adaptor_cont<Individual>::update_probability(std::vector<Individual> &pop, const std::vector<double>& weight, const std::vector<int> *index) {
			int popsize = pop.size();
			for (auto &i : m_proba) for (auto &j : i) {
				j.val = 0;
				j.idx.clear();
			}

			for (auto &i : m_limit) {
				i.boundary.first = std::numeric_limits<double>::max();
				i.boundary.second = std::numeric_limits<double>::lowest();
			}
			if (index == nullptr) {
				for (int i = 0; i < m_proba.size(); ++i) {
					for (int j = 0; j < popsize; ++j) {
						if (m_limit[i].boundary.first > pop[j].variable()[i])m_limit[i].boundary.first = pop[j].variable()[i];
						if (m_limit[i].boundary.second < pop[j].variable()[i])m_limit[i].boundary.second = pop[j].variable()[i];
					}
				}
			}
			else {
				for (int i = 0; i < m_proba.size(); ++i) {
					for (auto j : (*index)) {
						if (m_limit[i].boundary.first > pop[j].variable()[i])m_limit[i].boundary.first = pop[j].variable()[i];
						if (m_limit[i].boundary.second < pop[j].variable()[i])m_limit[i].boundary.second = pop[j].variable()[i];
					}
				}
			}

			if (index == nullptr) {
				for (int i = 0; i < m_proba.size(); ++i) {
					m_limit[i].range = m_limit[i].boundary.second - m_limit[i].boundary.first;
					if (m_limit[i].range > m_initialRange[i].range) m_initialRange[i].range = m_limit[i].range;
					m_limit[i].step = m_limit[i].range / m_num;
					if (m_limit[i].step > 0) {
						for (int j = 0; j < popsize; ++j) {
							int reg_idx = (pop[j].variable()[i] - m_limit[i].boundary.first) / m_limit[i].step;
							if (reg_idx == m_num) reg_idx--;
							m_proba[i][reg_idx].val += weight[j];
							m_proba[i][reg_idx].idx.push_back(j);

							m_pos[j][i] = reg_idx;
						}
					}
					else {
						for (int j = 0; j < popsize; ++j) {
							m_proba[i][0].val += weight[j];
							m_proba[i][0].idx.push_back(j);

							m_pos[j][i] = 0;
						}
					}
				}
			}
			else {
				for (int i = 0; i < m_proba.size(); ++i) {
					m_limit[i].range = m_limit[i].boundary.second - m_limit[i].boundary.first;
					if (m_limit[i].range > m_initialRange[i].range) m_initialRange[i].range = m_limit[i].range;
					m_limit[i].step = m_limit[i].range / m_num;
					if (m_limit[i].step > 0) {
						for (auto j : (*index)) {
							int reg_idx = (pop[j].variable()[i] - m_limit[i].boundary.first) / m_limit[i].step;
							if (reg_idx == m_num) reg_idx--;
							m_proba[i][reg_idx].val += weight[j];
							m_proba[i][reg_idx].idx.push_back(j);
							m_pos[j][i] = reg_idx;
						}
					}
					else {
						for (auto j : (*index)) {
							m_proba[i][0].val += weight[j];
							m_proba[i][0].idx.push_back(j);
							m_pos[j][i] = 0;
						}
					}
				}
			}

			accumlate_probability();

		}

		template<typename Individual>
		void adaptor_cont<Individual>::create_solution(std::vector<std::unique_ptr<Individual>> &pop, std::vector<Individual> &curPop, double alpha) {
			int popsize = pop.size();
			int numDim = m_proba.size();
			double x;
			domain<real> & range = CONTINOUS_CAST->range();

			for (int j = 0; j < popsize; j++) {
				if (!pop[j]->is_active()) continue;
				for (int i = 0; i < numDim; i++) {
					double p = global::ms_global->m_uniform[caller::Algorithm]->next();
					double center = 0, var = 0;
					if (alpha < 0)		//alpha = exp((m_limit[i].range / m_initialRange[i].range - 1));
						alpha = global::ms_global->m_uniform[caller::Algorithm]->next();
					if (p <= alpha) {
						center = pop[j]->variable()[i];
						int idex = (center - m_limit[i].boundary.first) / m_limit[i].step;
						if (idex < m_num&& idex >= 0) {
							for (auto k = 0; k < m_proba[i][idex].idx.size(); ++k) {
								var += m_limit[i].as[m_proba[i][idex].idx[k]];
							}
							var /= m_proba[i][idex].idx.size();
							if (m_proba[i][idex].idx.size() == 0) {
								std::cout << "error" << std::endl;
							}
						}
						else var = m_limit[i].step;
					}
					else {

						std::vector<double> pro(m_num), acc_pro(m_num);
						for (int k = 0; k < m_num; ++k) {
							pro[k] = m_proba[i][k].val*exp(-fabs(m_pos[j][i] - k) / m_num);
							acc_pro[k] = pro[k];
							if (k > 0) acc_pro[k] += acc_pro[k - 1];
						}

						p = global::ms_global->m_uniform[caller::Algorithm]->next()*acc_pro[m_num - 1];
						int idex = lower_bound(acc_pro.begin(), acc_pro.end(), p) - acc_pro.begin();


						/*p = global::ms_global->m_uniform[caller::Algorithm]->next()()*m_acc_proba[i][m_num - 1];
						int idex = lower_bound(m_acc_proba[i].begin(), m_acc_proba[i].end(), p) - m_acc_proba[i].begin();

						if (m_proba[i][idex].idx.size() == 0) {
							cout << "error" << endl;
						}*/

						for (auto k = 0; k < m_proba[i][idex].idx.size(); ++k) {
							center += pop[m_proba[i][idex].idx[k]]->variable()[i];
						}
						center /= m_proba[i][idex].idx.size();

						for (auto k = 0; k < m_proba[i][idex].idx.size(); ++k) {
							var += m_limit[i].as[m_proba[i][idex].idx[k]];
						}
						var /= m_proba[i][idex].idx.size();


						/*for (auto k = 0; k < m_proba[i][idex].idx.size(); ++k) {
							var += (pop[m_proba[i][idex].idx[k]]->variable()[i] - center)*(pop[m_proba[i][idex].idx[k]]->variable()[i] - center);
						}
						var = sqrt(var / m_proba[i][idex].idx.size());*/

					}
					if (center < range[i].limit.first) center = range[i].limit.first;
					else if (center > range[i].limit.second) center = range[i].limit.second;

					x = center + var* global::ms_global->m_cauchy[caller::Algorithm]->next();
					//x = center + var* global::ms_global->m_normal[caller::Algorithm]->next();
					

					if (x < range[i].limit.first) {
						x = (center + range[i].limit.first) / 2;
					}
					else if (x > range[i].limit.second) {
						x = (center + range[i].limit.second) / 2;
					}
					curPop[j].variable()[i] = x;
				}
			}
		}

		template<typename Individual>
		void adaptor_cont<Individual>::create_solution(std::vector<std::unique_ptr<Individual>> &pop, std::vector<Individual> &curPop, std::vector<Individual> &his, double alpha) {
			int popsize = pop.size();
			int numDim = m_proba.size();
			double x;
			domain<real> & range = CONTINOUS_CAST->range();

			for (int j = 0; j < popsize; j++) {
				for (int i = 0; i < numDim; i++) {
					double p = global::ms_global->m_uniform[caller::Algorithm]->next();
					double center = 0;
					if (alpha < 0)
						alpha = exp((m_limit[i].range / m_initialRange[i].range - 1));
					if (p <= alpha) {
						center = pop[j]->variable()[i];
					}
					else {
						p = global::ms_global->m_uniform[caller::Algorithm]->next()*m_acc_proba[i][m_num - 1];
						int idex = lower_bound(m_acc_proba[i].begin(), m_acc_proba[i].end(), p) - m_acc_proba[i].begin();

						for (auto k = 0; k < m_proba[i][idex].idx.size(); ++k) {
							center += his[m_proba[i][idex].idx[k]].variable()[i];
						}
						center /= m_proba[i][idex].idx.size();
					}
					if (center < range[i].limit.first) center = range[i].limit.first;
					else if (center > range[i].limit.second) center = range[i].limit.second;

					x = center + m_limit[i].step * global::ms_global->m_cauchy[caller::Algorithm]->next();
					//x = center + m_limit[i].step * Global::msp_global->mp_normalAlg->Next();
					//x =  Global::msp_global->mp_normalAlg->NextNonStand(center, m_limit[i].step*m_limit[i].step);
					if (x < range[i].limit.first) {
						x = (center + range[i].limit.first) / 2;
					}
					else if (x > range[i].limit.second) {
						x = (center + range[i].limit.second) / 2;
					}
					curPop[j].variable()[i] = x;
				}
			}
		}

		template<typename Individual>
		void adaptor_cont<Individual>::accumlate_probability() {

			for (auto i = 0; i < m_acc_proba.size(); ++i) {
				for (auto j = 0; j < m_acc_proba[i].size(); ++j) {
					m_acc_proba[i][j] = m_proba[i][j].val;
					if (j > 0) m_acc_proba[i][j] += m_acc_proba[i][j - 1];
				}
			}
		}

		template<typename Individual>
		evaluation_tag adaptor_cont<Individual>::update_solution(std::vector<std::unique_ptr<Individual>> &pop, std::vector<Individual> &curPop, int& impRadio, int ms) {

			evaluation_tag rf = evaluation_tag::Normal;
			int popsize = pop.size();
			impRadio = 0;
			for (int i = 0; i < popsize; i++)
			{
				if (!pop[i]->is_active()) continue;
				rf = curPop[i].evaluate();
				if (curPop[i].dominate(*pop[i])) {
					*pop[i] = curPop[i];
					pop[i]->set_improved_flag(true);
					impRadio++;
				}
				if (rf == evaluation_tag::Terminate) break;
			}

			return rf;
		}

		template<typename Individual>
		void adaptor_cont<Individual>::print_infor() {
			double mean1 = 0, mean2 = 0;
			for (auto i = 0; i < m_limit.size(); ++i) {
				mean1 += m_limit[i].range / m_initialRange[i].range;
				//mean2 += m_limit[i].range;
				//cout <<i<<" "<< m_limit[i].boundary.first << " " << m_limit[i].boundary.second << endl;

			}
			std::cout << std::setw(15) << mean1 / m_limit.size() << std::endl;
		}

		template<typename Individual>
		double adaptor_cont<Individual>::get_inter_distance() {
			double mean1 = 0;
			for (auto i = 0; i < m_limit.size(); ++i) {
				mean1 += m_limit[i].range / m_initialRange[i].range;
			}
			return mean1 / m_limit.size();
		}

		template<typename Individual>
		double adaptor_cont<Individual>::get_intra_distance(std::vector<std::unique_ptr<Individual>> &pop) {
			double mean = 0;
			int cnt = 0;
			for (int i = 0; i < m_proba.size(); ++i) {
				std::vector<double> x(pop.size());
				for (int j = 0; j < pop.size(); ++j) {
					x[j] = pop[j]->variable()[i];
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
			return mean / m_proba.size();
		}

		template<typename Individual>
		void adaptor_cont<Individual>::update_step(std::vector<std::unique_ptr<Individual>> &pop) {
			int popsize = pop.size();
			int numDim = m_proba.size();
			std::vector<double*> loc(popsize);
			std::vector<int> idx(popsize);
			for (int d = 0; d < numDim; ++d) {
				for (int i = 0; i < popsize; i++)
				{
					loc[i] = &pop[i]->variable()[d];
				}
				NS::quick_sort(loc, popsize, idx);
				double lb = CONTINOUS_CAST->range(d).first, ub = CONTINOUS_CAST->range(d).second;

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

		template<typename Individual>
		void adaptor_cont<Individual>::create_solution(std::vector<std::unique_ptr<Individual>> &pop, std::vector<Individual> &curPop, std::vector<double>& alpha) {
			int popsize = pop.size();
			int numDim = m_proba.size();
			real x;
			domain<real> & range = CONTINOUS_CAST->range();

			for (int j = 0; j < popsize; j++) {
				if (!pop[j]->is_active()) continue;
				for (int i = 0; i < numDim; i++) {
					double p = global::ms_global->m_uniform[caller::Algorithm]->next();
					double center = 0, var = 0;
					if (alpha[j] < 0)		alpha[j] = exp((m_limit[i].range / m_initialRange[i].range - 1));
					if (p <= alpha[j]) {
						center = pop[j]->variable()[i];
						int idex = (center - m_limit[i].boundary.first) / m_limit[i].step;
						if (idex < m_num&& idex >= 0) {
							for (auto k = 0; k < m_proba[i][idex].idx.size(); ++k) {
								var += m_limit[i].as[m_proba[i][idex].idx[k]];
							}
							var /= m_proba[i][idex].idx.size();
						}
						else var = m_limit[i].step;
					}
					else {
						p = global::ms_global->m_uniform[caller::Algorithm]->next()*m_acc_proba[i][m_num - 1];
						int idex = lower_bound(m_acc_proba[i].begin(), m_acc_proba[i].end(), p) - m_acc_proba[i].begin();

						for (auto k = 0; k < m_proba[i][idex].idx.size(); ++k) {
							center += pop[m_proba[i][idex].idx[k]]->variable()[i];
						}
						center /= m_proba[i][idex].idx.size();

						for (auto k = 0; k < m_proba[i][idex].idx.size(); ++k) {
							var += m_limit[i].as[m_proba[i][idex].idx[k]];
						}
						var /= m_proba[i][idex].idx.size();
					}
					if (center < range[i].limit.first) center = range[i].limit.first;
					else if (center > range[i].limit.second) center = range[i].limit.second;

					//x = center + var* global::ms_global->m_cauchy[caller::Algorithm]->next();
					x = center + var* global::ms_global->m_normal[caller::Algorithm]->next();

					if (x < range[i].limit.first) {
						x = (center + range[i].limit.first) / 2;
					}
					else if (x > range[i].limit.second) {
						x = (center + range[i].limit.second) / 2;
					}
					curPop[j].variable()[i] = x;
				}
			}
		}

		template<typename Individual>
		evaluation_tag adaptor_cont<Individual>::evenDistri(std::vector<std::unique_ptr<Individual>> &pop) {
			int popsize = pop.size();
			int numDim = m_proba.size();
			std::vector<bool> flag(popsize);
			std::list<std::vector<std::pair<double, double>>> box;
			std::list<std::list<int>> distri;
			int cnt = 0;
			while (cnt < popsize) {
				int idx = -1;
				while (flag.at(++idx));
				std::vector<std::pair<double, double>> b(numDim);
				for (int i = 0; i < numDim; ++i) {
					b[i].first = m_limit[i].boundary.first + floor((pop[idx]->variable()[i] - m_limit[i].boundary.first) / m_limit[i].step)*m_limit[i].step;
					b[i].second = b[i].first + m_limit[i].step;
				}
				std::list<int> indis;
				for (int i = idx; i < popsize; ++i) {
					if (!flag.at(i)) {
						bool inside = true;
						if (i != idx) {
							for (int d = 0; d < numDim; ++d) {
								if (b[d].first > pop[i]->variable()[d] || pop[i]->variable()[d] >= b[d].second) {
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

			std::list<int> crowd;
			std::cout << distri.size() << " " << avg << std::endl;
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
			evaluation_tag rf = evaluation_tag::Normal;
			while (crowd.size() > 0) {
				auto idx = box.begin();
				for (auto it = distri.begin(); it != distri.end(); ++it, ++idx) {
					if (crowd.size() == 0) break;
					if (it->size() < avg) {
						for (int d = 0; d < numDim; ++d) {
							double l1 = pop[it->front()]->variable()[d] - idx->at(d).first, l2 = idx->at(d).second - pop[it->front()]->variable()[d];
							l1 = l1 < l2 ? l1 : l2;

							double x = pop[it->front()]->variable()[d] + l1* global::ms_global->m_normal[caller::Algorithm]->next();
							if (x < idx->at(d).first) x = idx->at(d).first;
							else if (x > idx->at(d).second) x = idx->at(d).second;

							pop[crowd.front()]->variable()[d] = x;
						}
						rf = pop[crowd.front()]->evaluate();
						it->splice(it->end(), crowd, crowd.begin());
						if (rf != evaluation_tag::Normal) return rf;
					}
				}
			}
			return rf;
		}

		template<typename Individual>
		evaluation_tag adaptor_cont<Individual>::repel(std::vector<std::unique_ptr<Individual>> &pop) {
			int numDim = m_proba.size();

			std::map<std::vector<int>, std::vector<int>> cnt;
			for (auto i = 0; i < pop.size(); ++i) {
				cnt[m_pos[i]].push_back(i);
			}
			int mean = pop.size() / cnt.size();

			std::vector<int> redundant;
			std::vector<std::map<std::vector<int>, std::vector<int>>::iterator> insert;

			std::vector<bool> label(cnt.size(), false);
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


			evaluation_tag rf = evaluation_tag::Normal;

			for (auto &r : redundant) {
				int max_dis = std::numeric_limits<int>::max();
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
					double l = pop[best]->variable()[d] - lb;
					double r = rb - pop[best]->variable()[d];
					double var = l < r ? l : r, x;
					if (var == 0) {
						var = m_initialRange[d].range / pop.size();
					}
					x = pop[best]->variable()[d] + var*global::ms_global->m_normal[caller::Algorithm]->next();
					if (x < lb) x = (lb + pop[best]->variable()[d]) / 2;
					else if (x > rb) x = (rb + pop[best]->variable()[d]) / 2;
					pop[*itr]->variable()[d] = x;
				}
				(*i)->second.push_back(*itr);
				pop[*itr]->setActive(true);
				pop[*itr]->setFlag(true);

				rf = pop[*itr]->evaluate();
				if (rf != evaluation_tag::Normal) return rf;
			}

			while (itr != redundant.end()) {
				pop[*itr]->setActive(true);
				pop[*itr]->setFlag(true);
				rf = pop[*itr]->initialize();
				if (rf != evaluation_tag::Normal) return rf;
				++itr;
			}

			return rf;

		}
	}
}

#endif // !MDL_OPERATOR_CONTINUE
