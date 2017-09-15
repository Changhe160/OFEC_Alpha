#ifndef FAST_NONDOMINATED_SORT_H
#define FAST_NONDOMINATED_SORT_H

#include<map>
#include <list>
#include <utility>
#include "../../core/algorithm/encoding.h"
#include "../../core/definition.h"
#include <iostream>

using namespace std;
using namespace OFEC;

class fast_sort {
public:
	void sort() {

		for (int i = 0; i < m_popsize; i++) {
			rank[i] = -1;
		}

		auto i = cset.begin();
		for (int k = 0; k < m_popsize; k++, ++i) {
			for (int j = 0; j<m_popsize; j++) {
				if (k != j) {
					auto compare_result = m_compare(m_pop[j].second, m_pop[k].second, optimization_mode::Minimization);
					if (compare_result == dominationship::Dominating) {//*m_pop[j]>*m_pop[k]
						rank_[k]++;
					}
					else if (compare_result == dominationship::Dominated ) {//*m_pop[k]>*m_pop[j]
						(*i)[count[k]] = j;
						count[k]++;
					}
					m_num++;
				}
			}
		}

		int m_curRank = 0;
		vector<int> rank2(m_popsize);
		while (1)
		{
			int stop_count = 0;
			for (int k = 0; k<m_popsize; k++)
				rank2[k] = rank_[k];
			auto i = cset.begin();
			for (int k = 0; k<m_popsize; k++, ++i)
			{
				if (rank[k] == -1 && rank_[k] == 0)
				{
					rank[k] = m_curRank;
					for (int j = 0; j<count[k]; j++)
					{
						int id = (*i)[j];
						rank2[id]--;
						stop_count++;
					}
				}
			}

			for (int k = 0; k<m_popsize; k++)
				rank_[k] = rank2[k];

			m_curRank++;
			if (stop_count == 0)
				break;
		}
	}
	map<int, int>& ranking() {

		for (int i = 0; i < m_popsize; ++i) {
			m_label.emplace(m_pop[i].first, rank[i]);
		}
		return m_label;
	}
	int number() {
		return m_num;
	}
	fast_sort(vector<pair<int, vector<double>>> && data) :m_popsize(data.size()), m_pop(std::move(data)), rank_(m_popsize), rank(m_popsize), count(m_popsize),
		cset(m_popsize, vector<int>(m_popsize)) {}
	fast_sort(const vector<pair<int, vector<double>>> & data) :m_popsize(data.size()), m_pop(data), rank_(m_popsize), rank(m_popsize), count(m_popsize),
		cset(m_popsize, vector<int>(m_popsize)) {}
private:
	int m_popsize;
	vector<pair<int, vector<double>>> m_pop;
	vector<int> rank_, count, rank;
	list<vector<int> > cset;;
	objective_compare<> m_compare;
	int m_num = 0;

	map<int, int> m_label;
};


#endif // !FAST_NONDOMINATED_SORT_H