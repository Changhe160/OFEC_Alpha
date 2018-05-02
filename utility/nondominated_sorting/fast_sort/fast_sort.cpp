#include "fast_sort.h"

#ifdef USING_CONCURRENT
#include <thread>
#include <mutex>
#include <iostream>
std::mutex l_mutex;
#endif // USING_CONCURRENT

namespace NS {
	int fast_sort(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement)
	{
		std::chrono::time_point<std::chrono::system_clock> Total_start_time;
		std::chrono::microseconds Total_time_cost;
		Total_time_cost = Total_time_cost.zero();
		int NumComp(0);
		Total_start_time = std::chrono::system_clock::now();

		const std::size_t popsize(data.size());
		if (popsize == 0) return 0;
		if (rank.size() != popsize)
			rank.resize(popsize);
		const std::size_t objsnum(data.front().size());
		std::vector<int> rank_(popsize);
		std::vector<int> count(popsize);
		std::vector<std::vector<int>> cset(popsize, std::vector<int>(popsize));
		for (int i = 0; i < popsize; i++) {
			rank[i] = -1;
		}
        #ifdef USING_CONCURRENT
		int numTask = std::thread::hardware_concurrency();
		int TaskSize = popsize;
		if (numTask > TaskSize) numTask = TaskSize;
		std::vector<std::thread> thrd;
		for (int i = 0; i < numTask; ++i) {
			std::vector<int> ks;
			for (int k = i; k < TaskSize; k += numTask)
				ks.push_back(k);
			thrd.push_back(std::thread(ParallelCompare, popsize, std::move(ks), std::cref(data), std::ref(rank_), std::ref(count), std::ref(cset)));
		}
		for (auto&t : thrd) t.join();
        #else
		for (int k = 0; k < popsize; k++) {
			for (int j = 0; j<popsize; j++) {
				if (k != j) {
					auto compare_result = OFEC::objective_compare(data[j], data[k], OFEC::optimization_mode::Minimization);
					NumComp += compare_result.second;
					if (compare_result.first == OFEC::dominationship::Dominating) {//*data[j]>*data[k]
						rank_[k]++;
					}
					else if (compare_result.first == OFEC::dominationship::Dominated) {//*data[k]>*data[j]
						cset[k][count[k]] = j;
						count[k]++;
					}
				}
			}
		}
        #endif // USING_CONCURRENT
		int m_curRank = 0;
		std::vector<int> rank2(popsize);
		while (1)
		{
			int stop_count = 0;
			for (int k = 0; k<popsize; k++)
				rank2[k] = rank_[k];
			auto i = cset.begin();
			for (int k = 0; k<popsize; k++, ++i)
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
			for (int k = 0; k<popsize; k++)
				rank_[k] = rank2[k];
			m_curRank++;
			if (stop_count == 0) {
				int time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - Total_start_time).count();
				measurement.first += time;
				measurement.second += NumComp;
				return m_curRank;
			}
		}
	}
    #ifdef USING_CONCURRENT
	void ParallelCompare(int popsize, const std::vector<int>&& ks, const std::vector<std::vector<double>>& data, std::vector<int>& rank_, std::vector<int>& count, std::vector<std::vector<int>>& cset){
		for (int k : ks) {
			for (int j = 0; j<popsize; j++) {
				if (k != j) {
					auto compare_result = OFEC::objective_compare(data[j], data[k], OFEC::optimization_mode::Minimization);
					if (compare_result.first == OFEC::dominationship::Dominating) {//*data[j]>*data[k]
						rank_[k]++;
					}
					else if (compare_result.first == OFEC::dominationship::Dominated) {//*data[k]>*data[j]
						cset[k][count[k]] = j;
						count[k]++;
					}
				}
			}
		}
	}
    #endif // USING_CONCURRENT
}

