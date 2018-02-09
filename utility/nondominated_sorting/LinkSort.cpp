#include "LinkSort.h"
#include "../functional.h"
#include "quick_sort.h"
#include <algorithm>
#include <iostream>
#include <limits.h>
#include <string.h>
#ifdef USING_CONCURRENT
#include <thread>
#endif // USING_CONCURRENT

namespace NDS {
	void LinkSort(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement){

		std::chrono::time_point<std::chrono::system_clock> Total_start_time;
		std::chrono::microseconds Total_time_cost;
		Total_time_cost = Total_time_cost.zero();
		int NumComp(0);
		Total_start_time = std::chrono::system_clock::now();

		const int N = data.size(); // number of solution
		if (N == 0)
			return;
		const int m = data[0].size(); // number of obective
		if (rank.empty() || rank.size() != N)
			rank.resize(N);

		// ******************************************

		std::vector<std::vector<int>> SeqByObj(m);
        #ifdef USING_CONCURRENT
		int TaskSize = m;
		int numTask = std::thread::hardware_concurrency();
		if (numTask > TaskSize) numTask = TaskSize;
		std::vector<std::thread> thrd;
		for (int i = 0; i < numTask; ++i) {
			std::vector<int> ObjIdxs;
			for (int idx = i; idx < TaskSize; idx += numTask)
				ObjIdxs.push_back(idx);
			thrd.push_back(std::thread(ParallelQuickSort, std::cref(data), std::ref(SeqByObj), std::move(ObjIdxs)));
		}
		for (auto&t : thrd) 
			t.join();
        #else
		for (int i = 0; i < m; ++i) {
			NumComp += quick_sort(data, SeqByObj[i], i);
		}
        #endif // USING_CONCURRENT
		std::vector<LS_list> SeqByObj_Lists(m); // same as SeqByObj but in form of LS_list
		std::vector<std::vector<LS_node*>> PosInObjLists(N); // PosInObjLists[i] stores solution[i]'s all LS_node addresses 
		for (int i = 0; i < m; ++i) {
			for (int SolIdx : SeqByObj[i])
				PosInObjLists[SolIdx].push_back(SeqByObj_Lists[i].push_back(SolIdx));
		}

		std::vector<std::vector<int>> SolStas(N); // SolStas[i] stores solution[i]'s all single objective sequence numbers
		for (int i = 0; i < m; ++i) {
			int SeqNum = 0;
			for (LS_node* iter = SeqByObj_Lists[i].begin(); iter != nullptr; iter = iter->m_next) {
				SolStas[iter->m_value].push_back(SeqNum);
				SeqNum++;
			}
		}

		//std::vector<int> MinIdxs(N); // MinIdxs[i] means index of solution[i]'s minimum single objective sequence number
		//std::vector<int> SumVals(N); // SumVals[i] means sum of solution[i]'s all single objective sequence numbers
		//for (int i = 0; i < N; ++i) {
		//	int min_val = INT_MAX; // value of solution's minimum single objective sequence number
		//	int min_idx; // index of solution's minimum single objective sequence number
		//	for (int ObjIdx = 0; ObjIdx < m; ++ObjIdx) {
		//		if (SolStas[i][ObjIdx] < min_val) {
		//			min_val = SolStas[i][ObjIdx];
		//			min_idx = ObjIdx;
		//		}
		//		SumVals[i] += SolStas[i][ObjIdx];
		//	}
		//	MinIdxs[i] = min_idx;
		//}

		std::vector<int> MaxIdxs(N);
		std::vector<int> MinIdxs(N); // MinIdxs[i] means index of solution[i]'s minimum single objective sequence number
		std::vector<int> SumVals(N); // SumVals[i] means sum of solution[i]'s all single objective sequence numbers
		for (int i = 0; i < N; ++i) {
			int max_val = -1;
			int max_idx;
			int min_val = INT_MAX; // value of solution's minimum single objective sequence number
			int min_idx; // index of solution's minimum single objective sequence number
			for (int ObjIdx = 0; ObjIdx < m; ++ObjIdx) {
				if (SolStas[i][ObjIdx] < min_val) {
					min_val = SolStas[i][ObjIdx];
					min_idx = ObjIdx;
				}
				if (SolStas[i][ObjIdx] > max_val) {
					max_val = SolStas[i][ObjIdx];
					max_idx = ObjIdx;
				}
				SumVals[i] += SolStas[i][ObjIdx];
			}
			MinIdxs[i] = min_idx;
			MaxIdxs[i] = max_idx;
		}

		//std::vector<int> SumVals(N); // SumVals[i] means sum of solution[i]'s all single objective sequence numbers
		//for (int i = 0; i < N; ++i)
		//	for (int ObjIdx = 0; ObjIdx < m; ++ObjIdx) 
		//		SumVals[i] += SolStas[i][ObjIdx];
		//std::vector<std::vector<int>> MaxIdxss(N); //MinIdxs[i][j] means index of solution[i]'s [j]th maximum single objective sequence number
		//for (int i = 0; i < N; ++i)
		//	OFEC::quick_sort(SolStas[i], m, MaxIdxss[i]);

		std::vector<int> SeqBySumVals(N); // sequence of solution sorted by sum of all single objective sequence numbers
		OFEC::quick_sort(SumVals, N, SeqBySumVals);
		LS_list SeqBySumVals_Lists; // Same as SeqByMinVals but in form of LS_list
		for (const int SolIndex : SeqBySumVals)
			PosInObjLists[SolIndex].push_back(SeqBySumVals_Lists.push_back(SolIndex));

		// ******************************************

		int CurRankNumber = 0; // current rank number
		int num_not_ranked = N; // number of solutions not ranked
		std::vector<int> CurRankCandidate; // the candidate solutions of current rank
		CurRankCandidate.reserve(N);
		bool* InCurRankCandiate = new bool[N]; // whether in CurRankCandidate
		memset(InCurRankCandiate, false, N);
		int link; // the solution chosen to generate CurRankCandidate
		while (num_not_ranked > 0) {
			// set link
			link = SeqBySumVals_Lists.begin()->m_value;
			rank[link] = CurRankNumber;
			num_not_ranked--;
			// generate CurRankCandidate by link
			for (auto SeqByObj_List : SeqByObj_Lists) {
				for (auto iter = SeqByObj_List.begin(); iter != nullptr; iter = iter->m_next) {
					if (iter->m_value == link)
						break;
					else if (!InCurRankCandiate[iter->m_value]) {
						InCurRankCandiate[iter->m_value] = true;
						CurRankCandidate.push_back(iter->m_value);
					}
				}
			}
			// remove solution[link]'s all LS_nodes
			for (int i = 0; i < m; ++i)
				SeqByObj_Lists[i].erase(PosInObjLists[link][i]);
			SeqBySumVals_Lists.erase(PosInObjLists[link][m]);
			// filter the CurRankCandidate
#ifdef USING_CONCURRENT
			numTask = std::thread::hardware_concurrency();
			TaskSize = CurRankCandidate.size();
			if (numTask > TaskSize) numTask = TaskSize;
			thrd.clear();
			for (int i = 0; i < numTask; ++i) {
				std::vector<int> candidates;
				for (int idx = i; idx < TaskSize; idx += numTask)
					candidates.push_back(CurRankCandidate[idx]);
				thrd.push_back(std::thread(ParallelFilter, std::move(candidates), std::ref(SeqByObj_Lists), std::cref(MaxIdxs), std::cref(MinIdxs), m, std::cref(SolStas), InCurRankCandiate));
			}
			for (auto&t : thrd) 
				t.join();
#else
			for (auto candidate : CurRankCandidate) {
				bool FlagInCurRank(true); // whether candidate is in current rank 
				for (auto iter = SeqByObj_Lists[MinIdxs[candidate]].begin(); iter != nullptr; iter = iter->m_next) {
				//for (auto iter = SeqByObj_Lists[MaxIdxss[candidate][m - 1]].begin(); iter != nullptr; iter = iter->m_next) {
					if (iter->m_value == candidate)
						break;
					else {
						if (SolStas[iter->m_value][MaxIdxs[iter->m_value]] > SolStas[candidate][MaxIdxs[iter->m_value]])
							continue;
						// check whether solution[iter->m_value] donminate solution[candidate] or not
						bool FlagDominate(true);
						for (int i = 0; i < m; ++i) {
							if (i != MaxIdxs[iter->m_value] && SolStas[iter->m_value][i] > SolStas[candidate][i]) {
							//if (SolStas[iter->m_value][i] > SolStas[candidate][i]) {
							//int objidx = MaxIdxss[iter->m_value][i];
							//if (SolStas[iter->m_value][objidx] > SolStas[candidate][objidx]) {
								FlagDominate = false;
								break;
							}
						}
						if (FlagDominate) {
							FlagInCurRank = false;
							break;
						}
					}
				}
				if (!FlagInCurRank)
					InCurRankCandiate[candidate] = false;
			}
#endif // USING_CONCURRENT
			// set rank for filtered CurRankCandidate and remove their LS_nodes
			for (auto candidate : CurRankCandidate) {
				if (InCurRankCandiate[candidate]) {
					rank[candidate] = CurRankNumber;
					num_not_ranked--;
					for (int i = 0; i < m; ++i)
						SeqByObj_Lists[i].erase(PosInObjLists[candidate][i]);
					SeqBySumVals_Lists.erase(PosInObjLists[candidate][m]);
				}
			}
			// goto next rank
			CurRankNumber++;
			CurRankCandidate.clear();
			memset(InCurRankCandiate, false, N);
		}
		delete[] InCurRankCandiate;

		measurement.first += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - Total_start_time).count();
		measurement.second += NumComp;

	}
#ifdef USING_CONCURRENT
	void ParallelFilter(const std::vector<int>&& candidates, std::vector<LS_list>& SeqByObj_Lists, const std::vector<int>& MaxIdxs, const std::vector<int>& MinIdxs, const int m, const std::vector<std::vector<int>>& SolStas, bool* InCurRankCandiate) {
		for (int candidate : candidates) {
			bool FlagInCurRank(true); // whether candidate is in current rank 
			for (auto iter = SeqByObj_Lists[MinIdxs[candidate]].begin(); iter != nullptr; iter = iter->m_next) {
				if (iter->m_value == candidate)
					break;
				else {
					if (SolStas[iter->m_value][MaxIdxs[iter->m_value]] > SolStas[candidate][MaxIdxs[iter->m_value]])
						continue;
					// check whether solution[iter->m_value] donminate solution[candidate] or not
					bool FlagDominate(true);
					for (int i = 0; i < m; ++i)
						if (i != MaxIdxs[iter->m_value] && SolStas[iter->m_value][i] > SolStas[candidate][i]) {
						//if (SolStas[iter->m_value][i] > SolStas[candidate][i]) {
							FlagDominate = false;
							break;
						}
					if (FlagDominate) {
						FlagInCurRank = false;
						break;
					}
				}
			}
			if (!FlagInCurRank)
				InCurRankCandiate[candidate] = false;
		}
	}
	void ParallelQuickSort(const std::vector<std::vector<double>>& data, std::vector<std::vector<int>>& SeqByObj, const std::vector<int>&& ObjIdxs) {
		for (int ObjIdx : ObjIdxs)
			quick_sort(data, SeqByObj[ObjIdx], ObjIdx);
	}
#endif // USING_CONCURRENT
}