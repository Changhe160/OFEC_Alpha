#pragma once
#ifndef OPTIMUM_H
#define OPTIMUM_H
#include<algorithm>
#include "../algorithm/solution.h"
//#include "../Utility/TypeList/TypeManip.h"
namespace OFEC {
	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare < ObjetiveType >>
	class Optima {
		using solution_type = solution<VariableEncoding, ObjetiveType, ObjetiveCompare>;
	private:
		bool m_flagLoc, m_flagGloObj; // note: m_flagGloObj only for global optimum
		std::vector<bool> m_foundFlag;
		std::vector<solution_type> m_opt;	// note: the first optimum must be the global optimum for single obj prolems
		caller m_mode;
		bool m_recordFlag;

	public:
		Optima(int dim = 0, int numObj = 0, bool flagLoc = false, bool flagGObj = false, caller mode = Program_Problem);
		unsigned getNumOpt()const {
			return m_opt.size();
		}
		void setNumOpts(int num) {
			m_foundFlag.resize(num, false);
			m_opt.resize(num, m_opt[0]);
		}
		void setLocation(int i, const std::vector<ObjetiveType> &pos) {
			copy(pos.begin(), pos.end(), m_opt[i].data().m_x.begin());
		}
		void setLocation(int i, const std::vector<typevar> &pos) {
			m_opt[i].m_x = pos;
		}
		void setGloObj(const std::vector<std::vector<ObjetiveType>> &val) {
			if (val.size() != m_opt.size()) throw myException("@Optima<ED,T>::setGloObj()");
			for (int i = 0; i < val.size(); ++i) {
				copy(val[i].begin(), val[i].end(), m_opt[i].data().m_obj.begin());
			}
		}
		const std::vector<ObjetiveType>& getGloObj(int i = -1)const {
			if (i == -1 && m_opt[0].data().m_obj.size() == 1) {
				if (m_opt[0].data().m_obj.size()>1) {
					cout << "warning the first one in PF was returned" << endl;
				}
				return m_opt[0].data().m_obj;
			}
			else {
				return m_opt[i].data().m_obj;
			}
		}
		void appendOptima(const solution_type &opt, bool flag) {
			m_opt.push_back(move(opt));
			m_foundFlag.push_back(flag);
		}
		bool & flagGloObj() {
			return m_flagGloObj;
		}
		bool & flagLoc() {
			return m_flagLoc;
		}
		void setFlagLocTrue() {
			m_flagGloObj = true;
			m_flagLoc = true;
		}
		solution_type & operator[](const unsigned id) {
			return m_opt[id];
		}
		const solution_type & operator[](const unsigned id)const {
			return m_opt[id];
		}
		void clear() {
			m_opt.clear();
			m_foundFlag.clear();
		}
		void resizeDim(const int dim) {
			for (auto& i : m_opt) {
				i.data().m_x.resize(dim);
			}
		}
		void resizeObj(const int num) {
			for (auto& i : m_opt) {
				i.data().m_obj.resize(num);
			}
		}
		//template<typename TS>
		bool isFound(const solution_type &sol, double dis, double objAcc);
		int getNumGOptFound()const {
			int num = 0;
			for (auto i = m_foundFlag.begin(); i != m_foundFlag.end(); ++i) 	if (*i == true) ++num;
			return num;
		}
		//void printGOpt(ofstream & out)const;
		void setRecordFlag(bool f) {
			m_recordFlag = f;
		}
		void erase(int idx) {
			if (idx >= 0 && idx < m_opt.size())	m_opt.erase(m_opt.begin() + idx);
		}
		bool isAllFound()const {
			if (getNumGOptFound() == m_opt.size()) return true;
			return false;
		}
		const solution_type * getNearestGOpt(const solution_type& p, int *idx, double*dis)const {
			double mindis = numeric_limits<double>::max();
			int minidx = -1;
			for (int i = 0; i < m_opt.size(); ++i) {
				if (!m_foundFlag[i]) continue;
				double dis = m_opt[i].objective_distance(p);
				if (dis < mindis) {
					mindis = dis;
					minidx = i;
				}
			}
			if (minidx != -1) {
				if (idx) *idx = minidx;
				if (dis) *dis = mindis;
				return &m_opt[minidx];
			}
			else return 0;
		}
		void setFoundFlagFalse(int idx = -1) {
			if(idx == -1) for (size_t i = 0; i != m_foundFlag.size(); ++i)  m_foundFlag[i] = false;
		else m_foundFlag[idx] = false;
		}
		void setFoundFlagTrue(int idx = -1) {
			if (idx == -1) for (size_t i = 0; i != m_foundFlag.size(); ++i)  m_foundFlag[i] = true;
			else m_foundFlag[idx] = true;
		}
		bool isFound(const std::vector<ObjetiveType> &obj) {
			if (!m_recordFlag) return false;

			if (m_flagGloObj) {
				if (m_opt[0].objective_distance(obj) == 0) {
					setFoundFlagTrue();
					return true;
				}
			}

			return false;
		}
	};

	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare < ObjetiveType >>
	Optima<VariableEncoding, ObjetiveType, ObjetiveCompare>::Optima(int dim, int numObj, bool flagLoc, bool flagObj, caller mode) :m_flagLoc(flagLoc), m_flagGloObj(flagObj), m_mode(mode), m_recordFlag(true) {
		if (m_mode == caller::Algorithm) {
			m_flagLoc = false;
			m_flagGloObj = false;
			return;
		}
		if (dim > 0 && numObj > 0) {
			//bool check[Loki::SuperSubclass<Solution<ED>, T>::value]; // check if T is derived from Solution or is type Solution
			m_opt.push_back(move(T(dim, numObj)));
			m_foundFlag.push_back(false);
		}
	}

	template<typename VariableEncoding = variable<real>,
		typename ObjetiveType = real,
		typename ObjetiveCompare = objective_compare < ObjetiveType >>
	//template<typename TS>
	bool Optima<VariableEncoding, ObjetiveType, ObjetiveCompare>::isFound(const solution_type &sol, double disAcc, double objAcc) {
		if (!m_recordFlag) return false;
		bool flag = true;
		if (m_mode == caller::Problem) {
			if (m_flagGloObj&&!m_flagLoc) { // case: only gopt function value is provided, loc of gopt isnot provided				
				if (m_opt[0].objective_distance(sol) > objAcc) {  
					flag = false;
				}
			}
			else if (m_flagGloObj&&m_flagLoc) {// case: both gopt and lopt are provided
				std::vector<double> dis;
				for (auto&i : m_opt) dis.push_back(i.objective_distance(sol));
				if (m_opt[std::min_element(dis.begin(), dis.end()) - dis.begin()].objective_distance(sol) > objAcc) flag = false;
			}
			else {
				flag = false;
			}
		}

		if (flag) {
			if (m_flagLoc) {
				for (decltype (m_opt.size()) i = 0; i < m_opt.size(); ++i) {
					if (m_foundFlag[i]) continue;
					double d = m_opt[i].objective_distance(sol);
					if (d <= disAcc) {
						m_foundFlag[i] = true;
						return true;
					}
				}
				return false;
			}
			else {
				int numFound = getNumGOptFound();
				for (int i = 0; i < numFound; ++i) {
					if (m_opt[i].objective_distance(sol) <= disAcc) {
						flag = false;  // peak i has already been found
						break;
					}
				}
				if (flag) {
					if (m_mode == caller::Algorithm) {
						m_opt.push_back(move(sol));
						m_foundFlag.push_back(true);
					}
					else {
						if (numFound < m_opt.size()) {
							copy(sol.m_x.begin(), sol.m_x.end(), m_opt[numFound].data().m_x.begin());
							m_foundFlag[numFound] = true;
						}
						else {
							cout << "please increase distance accuracy to the global optima" << endl;
						}
					}
				}
			}
		}
		return flag;
	}

}
#endif