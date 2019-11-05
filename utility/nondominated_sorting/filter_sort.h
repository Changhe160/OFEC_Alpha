#ifndef OFEC_FILTER_SORT_H
#define OFEC_FILTER_SORT_H

#include <vector>
#include <utility>
#include <thread>
#include "../functional.h"
#include <cstring>

namespace OFEC {
	namespace NS {
		namespace FilterSort {
			template<typename T>
			bool greater(const std::vector<T>* a, const std::vector<T>* b, const int obj_idx) {
				size_t obj_num = a->size();
				for (size_t i = 0; i < obj_num; ++i) {
					int idx = (i + obj_idx) % obj_num;
					if (a->at(idx) > b->at(idx))
						return true;
					else if (a[idx] < b[idx])
						return false;
				}
				return true;
			}

			template<typename T>
			void merge(const std::vector<std::vector<T>*>& data, const int obj_idx, std::vector<int>& A, std::vector<int>& B, int l, int m, int r) {
				int i = l, j = m;
				for (int k = l; k < r; ++k) {
					if (i < m && (j >= r || !greater<T>(data[A[i]], data[A[j]], obj_idx))) {
						B[k] = A[i];
						i++;
					}
					else {
						B[k] = A[j];
						j++;
					}
				}
			}

			template<typename T>
			void mergeSort(const std::vector<std::vector<T>*>& data, const int obj_idx, std::vector<int>& B, std::vector<int>& A, int l, int r)
			{
				if (r - l < 2)
					return;
				int m = (r + l) / 2;
				mergeSort<T>(data, obj_idx, A, B, l, m);
				mergeSort<T>(data, obj_idx, A, B, m, r);

				merge<T>(data, obj_idx, B, A, l, m, r);
			}

			template<typename T>
			void merge_sort(const std::vector<std::vector<T>*>& data, std::vector<int>& index, const int obj_idx, bool ascending = true, int low = -1, int high = -1) {
				if (low == -1) {
					low = 0;
					high = data.size() - 1;
				}
				const int N = high - low + 1;
				if (index.size() == 0 || index.size() != N)		index.resize(N);

				std::vector<int> A(data.size());
				for (auto i = A.begin(); i != A.end(); ++i)* i = i - A.begin();
				std::vector<int> B(A);
				mergeSort<T>(data, obj_idx, B, A, low, high + 1);
				for (int i = 0; i < N; ++i)
					index[i] = A[i + low];
				if (!ascending) {
					for (size_t i = 0; i < N / 2; i++)
						std::swap(index[i], index[N - 1 - i]);
				}
			}

			template<typename T>
			void parallel_mergesort(const std::vector<std::vector<T>*>& data, std::vector<std::vector<int>>& SeqByObj, const std::vector<int>&& ObjIdxs, const std::vector<optimization_mode>& opt_mode) {
				for (int ObjIdx : ObjIdxs)
					merge_sort<T>(data, SeqByObj[ObjIdx], ObjIdx, opt_mode[ObjIdx] == optimization_mode::Minimization);
			}

			struct Node {
				Node(const int value, Node* last = nullptr, Node* next = nullptr) : m_value(value), m_last(last), m_next(next) {}
				const int m_value;
				Node* m_last;
				Node* m_next;
			};

			class List {
			public:
				List() : m_begin(nullptr), m_end(nullptr) {}
				~List() {
					while (m_end != nullptr)
					{
						erase(m_end);
					}
				}
				Node* push_back(const int value) {
					Node* new_node(new Node(value));
					if (m_begin != nullptr) {
						new_node->m_last = m_end;
						m_end->m_next = new_node;
						m_end = new_node;
					}
					else {
						m_begin = new_node;
						m_end = new_node;
					}
					return new_node;
				}
				void erase(Node* Node) {
					if (Node != m_begin && Node != m_end) {
						Node->m_last->m_next = Node->m_next;
						Node->m_next->m_last = Node->m_last;
					}
					else if (Node == m_begin && Node != m_end) {
						Node->m_next->m_last = nullptr;
						m_begin = Node->m_next;
					}
					else if (Node == m_end && Node != m_begin) {
						Node->m_last->m_next = nullptr;
						m_end = Node->m_last;
					}
					else {
						m_begin = nullptr;
						m_end = nullptr;
					}
					delete Node;
				}
				Node* begin() { return m_begin; }
				Node* end() { return m_end; }
				const Node* begin() const { return m_begin; }
				const Node* end() const { return m_end; }
				bool empty() const { return m_begin == nullptr; }
			private:
				Node* m_begin;
				Node* m_end;
			};

			inline void parallel_filter(const std::vector<int>&& candidates, const std::vector<List>& SeqByObj_Lists, const std::vector<int>& MaxIdxs, const std::vector<int>& MinIdxs, const int m, const std::vector<std::vector<int>>& SolStas, bool* InCurFront) {
				for (int candidate : candidates) {
					bool FlagInCurRank(true); // whether candidate is in current rank 
					for (const Node* iter = SeqByObj_Lists[MinIdxs[candidate]].begin(); iter != nullptr; iter = iter->m_next) {
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
						InCurFront[candidate] = false;
				}
			}

		}


		template<typename T>
		int filter_sort_p(const std::vector<std::vector<T>*>& data, std::vector<int>& rank, const std::vector<optimization_mode>& opt_mode, size_t num_task) {
			const int N = data.size(); // number of solution
			if (N == 0)	return -1;
			const int M = data[0]->size(); // number of obective
			if (rank.empty() || rank.size() != N)
				rank.resize(N);
			std::vector<std::vector<int>> SeqByObj(M);

			int TaskSize = M;
			std::vector<std::thread> thrd;
			for (int i = 0; i < num_task; ++i) {
				std::vector<int> ObjIdxs;
				for (int idx = i; idx < TaskSize; idx += num_task)
					ObjIdxs.push_back(idx);
				thrd.push_back(std::thread(FilterSort::parallel_mergesort<T>, std::cref(data), std::ref(SeqByObj), std::move(ObjIdxs), std::cref(opt_mode)));
			}
			for (auto& t : thrd)
				t.join();

			std::vector<FilterSort::List> SeqByObj_Lists(M); // same as SeqByObj but in form of LS_list
			std::vector<std::vector<FilterSort::Node*>> PosInObjLists(N); // PosInObjLists[i] stores solution[i]'s all LS_node addresses 
			for (int i = 0; i < M; ++i) {
				for (int SolIdx : SeqByObj[i])
					PosInObjLists[SolIdx].push_back(SeqByObj_Lists[i].push_back(SolIdx));
			}
			std::vector<std::vector<int>> SolStas(N); // SolStas[i] stores solution[i]'s all single objective sequence numbers
			for (int i = 0; i < M; ++i) {
				int SeqNum = 0;
				for (FilterSort::Node* iter = SeqByObj_Lists[i].begin(); iter != nullptr; iter = iter->m_next) {
					SolStas[iter->m_value].push_back(SeqNum);
					SeqNum++;
				}
			}

			std::vector<int> MaxIdxs(N); // MaxIdxs[i] means index of solution[i]'s maximum objective sequence number
			std::vector<int> MinIdxs(N); // MinIdxs[i] means index of solution[i]'s minimum objective sequence number
			std::vector<int> SumVals(N); // SumVals[i] means sum of solution[i]'s all objective sequence numbers
			for (int i = 0; i < N; ++i) {
				int max_val = -1;
				int min_val = N; // value of solution's minimum single objective sequence number
				for (int ObjIdx = 0; ObjIdx < M; ++ObjIdx) {
					if (SolStas[i][ObjIdx] < min_val) {
						min_val = SolStas[i][ObjIdx];
						MinIdxs[i] = ObjIdx;
					}
					if (SolStas[i][ObjIdx] > max_val) {
						max_val = SolStas[i][ObjIdx];
						MaxIdxs[i] = ObjIdx;
					}
					SumVals[i] += SolStas[i][ObjIdx];
				}
			}

			std::vector<int> SeqBySumVals(N); // sequence of solution sorted by sum of all single objective sequence numbers
			merge_sort(SumVals, N, SeqBySumVals);
			FilterSort::List SeqBySumVals_Lists; // Same as SeqByMinVals but in form of LS_list
			for (const int SolIndex : SeqBySumVals)
				PosInObjLists[SolIndex].push_back(SeqBySumVals_Lists.push_back(SolIndex));
			int CurRankNumber = 0; // current rank number
			int num_not_ranked = N; // number of solutions not ranked
			std::vector<int> CurCandidate; // the candidate solutions of current front
			CurCandidate.reserve(N);
			bool* InCurFront = new bool[N]; // whether in current front
			memset(InCurFront, false, N);
			int filter; // the solution chosen to generate CurRankCandidate
			while (num_not_ranked > 0) {
				// set filter
				filter = SeqBySumVals_Lists.begin()->m_value;
				// generate CurCandidate by filter
				for (auto& SeqByObj_List : SeqByObj_Lists) {
					for (auto iter = SeqByObj_List.begin(); iter != nullptr; iter = iter->m_next) {
						if (iter->m_value == filter)
							break;
						else if (!InCurFront[iter->m_value]) {
							InCurFront[iter->m_value] = true;
							CurCandidate.push_back(iter->m_value);
						}
					}
				}
				// filter the CurCandidate
				TaskSize = CurCandidate.size();
				thrd.clear();
				for (int i = 0; i < num_task; ++i) {
					std::vector<int> candidates;
					for (int idx = i; idx < TaskSize; idx += num_task)
						candidates.push_back(CurCandidate[idx]);
					thrd.push_back(std::thread(FilterSort::parallel_filter, std::move(candidates), std::cref(SeqByObj_Lists), std::cref(MaxIdxs), std::cref(MinIdxs), M, std::cref(SolStas), InCurFront));
				}
				for (auto& t : thrd)
					t.join();
				// set rank for current Front
				std::vector<int> curFront;
				for (auto candidate : CurCandidate) {
					if (InCurFront[candidate]) {
						rank[candidate] = CurRankNumber;
						num_not_ranked--;
						curFront.push_back(candidate);
					}
				}
				rank[filter] = CurRankNumber;
				num_not_ranked--;
				curFront.push_back(filter);
				// if terminate
				if (num_not_ranked == 0)
					break;
				// delete ranked solutions from lists
				for (auto candidate : curFront) {
					for (int i = 0; i < M; ++i)
						SeqByObj_Lists[i].erase(PosInObjLists[candidate][i]);
					SeqBySumVals_Lists.erase(PosInObjLists[candidate][M]);
				}
				// goto next rank
				CurRankNumber++;
				CurCandidate.clear();
				memset(InCurFront, false, N);
			}
			delete[] InCurFront;
		}


	}
}
#endif // !OFEC_FILTER_SORT_H

