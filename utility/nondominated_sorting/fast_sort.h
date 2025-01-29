#ifndef FAST_NONDOMINATED_SORT_H
#define FAST_NONDOMINATED_SORT_H

#include "../functional.h"
#include <functional>
#include <thread>

namespace ofec {
	namespace nd_sort {
		template<typename ObjectiveType>
		int fastSort(const std::vector<std::vector<ObjectiveType>*>& data, std::vector<int>& rank, const std::vector<OptimizeMode>& opt_mode) {
			const std::size_t popsize(data.size());
			if (popsize == 0) return 0;
			if (rank.size() != popsize)
				rank.resize(popsize);
			std::vector<int> rank_(popsize);
			std::vector<int> count(popsize);
			std::vector<std::vector<int>> cset(popsize, std::vector<int>(popsize));

			for (int i = 0; i < popsize; i++)
				rank[i] = -1;

			for (int k = 0; k < popsize; k++) {
				for (int j = 0; j < popsize; j++) {
					if (k != j) {
						auto compare_result = objectiveCompare<ObjectiveType>(*data[j], *data[k], opt_mode);
						if (compare_result == Dominance::kDominant) {//*data[j]>*data[k]
							rank_[k]++;
						}
						else if (compare_result == Dominance::kDominated) {//*data[k]>*data[j]
							cset[k][count[k]] = j;
							count[k]++;
						}
					}
				}
			}

			int m_curRank = 0;
			std::vector<int> rank2(popsize);
			while (1) {
				int stop_count = 0;
				for (int k = 0; k < popsize; k++)
					rank2[k] = rank_[k];
				auto i = cset.begin();
				for (int k = 0; k < popsize; k++, ++i) {
					if (rank[k] == -1 && rank_[k] == 0) {
						rank[k] = m_curRank;
						for (int j = 0; j < count[k]; j++) {
							int id = (*i)[j];
							rank2[id]--;
							stop_count++;
						}
					}
				}
				for (int k = 0; k < popsize; k++)
					rank_[k] = rank2[k];
				m_curRank++;
				if (stop_count == 0) {
					return m_curRank;
				}
			}
		}

		template<typename ObjectiveType>
		int fastSort(const std::vector<std::vector<ObjectiveType>>& data, std::vector<int>& rank, const std::vector<OptimizeMode>& opt_mode) {
			const std::size_t popsize(data.size());
			if (popsize == 0) return 0;
			if (rank.size() != popsize)
				rank.resize(popsize);
			std::vector<int> rank_(popsize);
			std::vector<int> count(popsize);
			std::vector<std::vector<int>> cset(popsize, std::vector<int>(popsize));

			for (int i = 0; i < popsize; i++)
				rank[i] = -1;

			for (int k = 0; k < popsize; k++) {
				for (int j = 0; j < popsize; j++) {
					if (k != j) {
						auto compare_result = objectiveCompare<ObjectiveType>(data[j], data[k], opt_mode);
						if (compare_result == Dominance::kDominant) {//*data[j]>*data[k]
							rank_[k]++;
						}
						else if (compare_result == Dominance::kDominated) {//*data[k]>*data[j]
							cset[k][count[k]] = j;
							count[k]++;
						}
					}
				}
			}

			int m_curRank = 0;
			std::vector<int> rank2(popsize);
			while (1) {
				int stop_count = 0;
				for (int k = 0; k < popsize; k++)
					rank2[k] = rank_[k];
				auto i = cset.begin();
				for (int k = 0; k < popsize; k++, ++i) {
					if (rank[k] == -1 && rank_[k] == 0) {
						rank[k] = m_curRank;
						for (int j = 0; j < count[k]; j++) {
							int id = (*i)[j];
							rank2[id]--;
							stop_count++;
						}
					}
				}
				for (int k = 0; k < popsize; k++)
					rank_[k] = rank2[k];
				m_curRank++;
				if (stop_count == 0) {
					return m_curRank;
				}
			}
		}

		template<typename ObjectiveType>
		void fast_sort_p_compare(const std::vector<std::vector<ObjectiveType>*>& data, const std::vector<OptimizeMode>& opt_mode, int popsize, const std::vector<int>&& ks, std::vector<int>& rank_, std::vector<int>& count, std::vector<std::vector<int>>& cset) {
			for (int k : ks) {
				for (int j = 0; j < popsize; j++) {
					if (k != j) {
						auto compare_result = objectiveCompare<ObjectiveType>(*data[j], *data[k], opt_mode);
						if (compare_result == Dominance::kDominant) {//*data[j]>*data[k]
							rank_[k]++;
						}
						else if (compare_result == Dominance::kDominated) {//*data[k]>*data[j]
							cset[k][count[k]] = j;
							count[k]++;
						}
					}
				}
			}
		}

		template<typename ObjectiveType>
		int fastSortP(const std::vector<std::vector<ObjectiveType>*>& data, std::vector<int>& rank, const std::vector<OptimizeMode>& opt_mode, int num_task = -1) {
			if (num_task == -1)
				num_task = std::thread::hardware_concurrency();
			const std::size_t popsize(data.size());
			if (popsize == 0) return 0;
			if (rank.size() != popsize)
				rank.resize(popsize);
			std::vector<int> rank_(popsize);
			std::vector<int> count(popsize);
			std::vector<std::vector<int>> cset(popsize, std::vector<int>(popsize));

			for (int i = 0; i < popsize; i++)
				rank[i] = -1;

			int TaskSize = popsize;
			if (num_task > TaskSize) num_task = TaskSize;
			std::vector<std::thread> thrd;
			for (int i = 0; i < num_task; ++i) {
				std::vector<int> ks;
				for (int k = i; k < TaskSize; k += num_task)
					ks.push_back(k);
				thrd.push_back(std::thread(fast_sort_p_compare<ObjectiveType>, std::ref(data), std::cref(opt_mode), popsize, std::move(ks), std::ref(rank_), std::ref(count), std::ref(cset)));
			}
			for (auto& t : thrd) t.join();


			int m_curRank = 0;
			std::vector<int> rank2(popsize);
			while (1)
			{
				int stop_count = 0;
				for (int k = 0; k < popsize; k++)
					rank2[k] = rank_[k];
				auto i = cset.begin();
				for (int k = 0; k < popsize; k++, ++i) {
					if (rank[k] == -1 && rank_[k] == 0) {
						rank[k] = m_curRank;
						for (int j = 0; j < count[k]; j++) {
							int id = (*i)[j];
							rank2[id]--;
							stop_count++;
						}
					}
				}
				for (int k = 0; k < popsize; k++)
					rank_[k] = rank2[k];
				m_curRank++;
				if (stop_count == 0) {
					return m_curRank;
				}
			}
		}

		template<typename Solution>
		int fastSort(const std::vector<Solution>& data, std::vector<int>& rank, std::function<Dominance(const Solution&, const Solution&)>& comp) {
			const std::size_t popsize(data.size());
			if (popsize == 0) return 0;
			if (rank.size() != popsize)
				rank.resize(popsize);
			std::vector<int> rank_(popsize);
			std::vector<int> count(popsize);
			std::vector<std::vector<int>> cset(popsize, std::vector<int>(popsize));

			for (int i = 0; i < popsize; i++)
				rank[i] = -1;

			for (int k = 0; k < popsize; k++) {
				for (int j = 0; j < popsize; j++) {
					if (k != j) {
						auto compare_result = comp(data[j], data[k]);
						if (compare_result == Dominance::kDominant) {//*data[j]>*data[k]
							rank_[k]++;
						}
						else if (compare_result == Dominance::kDominated) {//*data[k]>*data[j]
							cset[k][count[k]] = j;
							count[k]++;
						}
					}
				}
			}

			int m_curRank = 0;
			std::vector<int> rank2(popsize);
			while (1) {
				int stop_count = 0;
				for (int k = 0; k < popsize; k++)
					rank2[k] = rank_[k];
				auto i = cset.begin();
				for (int k = 0; k < popsize; k++, ++i) {
					if (rank[k] == -1 && rank_[k] == 0) {
						rank[k] = m_curRank;
						for (int j = 0; j < count[k]; j++) {
							int id = (*i)[j];
							rank2[id]--;
							stop_count++;
						}
					}
				}
				for (int k = 0; k < popsize; k++)
					rank_[k] = rank2[k];
				m_curRank++;
				if (stop_count == 0) {
					return m_curRank;
				}
			}
		}

		

		
	}
}





#endif // !FAST_NONDOMINATED_SORT_H