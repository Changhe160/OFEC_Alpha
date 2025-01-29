#ifndef OFEC_FILTER_SORT_H
#define OFEC_FILTER_SORT_H

#include <vector>
#include <utility>
#include <thread>
#include "../functional.h"
#include <cstring>

namespace ofec {
	namespace nd_sort {
		namespace filter_sort {
			template<typename T>
			bool greater(const std::vector<T>* a, const std::vector<T>* b, const int obj_idx, const std::vector<OptimizeMode>& opt_mode) {
				size_t obj_num = a->size();
				for (size_t i = 0; i < obj_num; ++i) {
					int idx = (i + obj_idx) % obj_num;
					if (opt_mode[idx] == OptimizeMode::kMinimize) {
						if (a->at(idx) > b->at(idx))
							return true;
						else if (a->at(idx) < b->at(idx))
							return false;
					}
					else {
						if (a->at(idx) < b->at(idx))
							return true;
						else if (a->at(idx) > b->at(idx))
							return false;
					}
				}
				return true;
			}

			template<typename T>
			void merge(const std::vector<std::vector<T>*>& data, const int obj_idx, std::vector<int>& A, std::vector<int>& B, int l, int m, int r, const std::vector<OptimizeMode>& opt_mode) {
				int i = l, j = m;
				for (int k = l; k < r; ++k) {
					if (i < m && (j >= r || !greater<T>(data[A[i]], data[A[j]], obj_idx, opt_mode))) {
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
			void mergeSortColRecur(const std::vector<std::vector<T>*>& data, const int obj_idx, std::vector<int>& B, std::vector<int>& A, int l, int r, const std::vector<OptimizeMode>& opt_mode) {
				if (r - l < 2)
					return;
				int m = (r + l) / 2;
				mergeSortColRecur<T>(data, obj_idx, A, B, l, m, opt_mode);
				mergeSortColRecur<T>(data, obj_idx, A, B, m, r, opt_mode);

				merge<T>(data, obj_idx, B, A, l, m, r, opt_mode);
			}

			template<typename T>
			void mergeSortCol(const std::vector<std::vector<T>*>& data, std::vector<int>& index, const int obj_idx, const std::vector<OptimizeMode>& opt_mode) {
				const int N = data.size();
				if (index.size() == 0 || index.size() != N)		index.resize(N);

				std::vector<int> A(data.size());
				for (auto i = A.begin(); i != A.end(); ++i)* i = i - A.begin();
				std::vector<int> B(A);
				mergeSortColRecur<T>(data, obj_idx, B, A, 0, N, opt_mode);
				for (int i = 0; i < N; ++i)
					index[i] = A[i];
			}

			template<typename T>
			void parallelMergeSort(const std::vector<std::vector<T>*>& data, std::vector<std::vector<int>>& SeqByObj, std::vector<int> ObjIdxs, const std::vector<OptimizeMode>& opt_mode) {
				for (int ObjIdx : ObjIdxs)
					mergeSortCol<T>(data, SeqByObj[ObjIdx], ObjIdx, opt_mode);
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

			inline void parallelFilter(std::vector<int> candidates, const std::vector<List>& seq_by_obj_lists, const std::vector<int>& max_idxs, const std::vector<int>& MinIdxs, const int m, const std::vector<std::vector<int>>& SolStas, bool* InCurFront) {
				for (int candidate : candidates) {
					bool flag_in_cur_rank(true); // whether candidate is in current rank 
					for (const Node* iter = seq_by_obj_lists[MinIdxs[candidate]].begin(); iter != nullptr; iter = iter->m_next) {
						if (iter->m_value == candidate)
							break;
						else {
							if (SolStas[iter->m_value][max_idxs[iter->m_value]] > SolStas[candidate][max_idxs[iter->m_value]])
								continue;
							// check whether solution[iter->m_value] donminate solution[candidate] or not
							bool flag_dominate(true);
							for (int i = 0; i < m; ++i)
								if (i != max_idxs[iter->m_value] && SolStas[iter->m_value][i] > SolStas[candidate][i]) {
									//if (sol_stas[iter->m_value][i] > sol_stas[candidate][i]) {
									flag_dominate = false;
									break;
								}
							if (flag_dominate) {
								flag_in_cur_rank = false;
								break;
							}
						}
					}
					if (!flag_in_cur_rank)
						InCurFront[candidate] = false;
				}
			}

		}

		template<typename T>
		int filterSort(const std::vector<std::vector<T>*>& data, 
			std::vector<int>& rank, 
			const std::vector<OptimizeMode>& opt_mode) {
			const int N = data.size(); // number of solution
			if (N == 0)	return -1;
			const int M = data[0]->size(); // number of obective
			if (rank.empty() || rank.size() != N)
				rank.resize(N);
			std::vector<std::vector<int>> seq_by_obj(M);

			for (size_t j = 0; j < M; j++) {
				filter_sort::mergeSortCol<T>(data, seq_by_obj[j], j, opt_mode);
			}

			std::vector<filter_sort::List> seq_by_obj_lists(M); // same as seq_by_obj but in form of LS_list
			std::vector<std::vector<filter_sort::Node*>> pos_in_obj_lists(N); // pos_in_obj_lists[i] stores solution[i]'s all LS_node addresses 
			for (int i = 0; i < M; ++i) {
				for (int sol_idx : seq_by_obj[i])
					pos_in_obj_lists[sol_idx].push_back(seq_by_obj_lists[i].push_back(sol_idx));
			}
			std::vector<std::vector<int>> sol_stas(N); // sol_stas[i] stores solution[i]'s all single objective sequence numbers
			for (int i = 0; i < M; ++i) {
				int seq_num = 0;
				for (filter_sort::Node* iter = seq_by_obj_lists[i].begin(); iter != nullptr; iter = iter->m_next) {
					sol_stas[iter->m_value].push_back(seq_num);
					seq_num++;
				}
			}

			std::vector<int> max_idxs(N); // max_idxs[i] means index of solution[i]'s maximum objective sequence number
			std::vector<int> min_idxs(N); // min_idxs[i] means index of solution[i]'s minimum objective sequence number
			std::vector<int> sum_vals(N); // sum_vals[i] means sum of solution[i]'s all objective sequence numbers
			for (int i = 0; i < N; ++i) {
				int max_val = -1;
				int min_val = N; // value of solution's minimum single objective sequence number
				for (int obj_idx = 0; obj_idx < M; ++obj_idx) {
					if (sol_stas[i][obj_idx] < min_val) {
						min_val = sol_stas[i][obj_idx];
						min_idxs[i] = obj_idx;
					}
					if (sol_stas[i][obj_idx] > max_val) {
						max_val = sol_stas[i][obj_idx];
						max_idxs[i] = obj_idx;
					}
					sum_vals[i] += sol_stas[i][obj_idx];
				}
			}

			std::vector<int> seq_by_sum_vals(N); // sequence of solution sorted by sum of all single objective sequence numbers
			mergeSort(sum_vals, N, seq_by_sum_vals);
			filter_sort::List seq_by_sum_vals__lists; // Same as SeqByMinVals but in form of LS_list
			for (const int sol_index : seq_by_sum_vals)
				pos_in_obj_lists[sol_index].push_back(seq_by_sum_vals__lists.push_back(sol_index));
			int cur_rank_number = 0; // current rank number
			int num_not_ranked = N; // number of solutions not ranked
			std::vector<int> cur_candidate; // the candidate solutions of current front
			cur_candidate.reserve(N);
			bool* in_cur_front = new bool[N]; // whether in current front
			memset(in_cur_front, false, N);
			int filter; // the solution chosen to generate CurRankCandidate
			while (num_not_ranked > 0) {
				// set filter
				filter = seq_by_sum_vals__lists.begin()->m_value;
				// generate cur_candidate by filter
				for (auto& seq_by_obj_list : seq_by_obj_lists) {
					for (auto iter = seq_by_obj_list.begin(); iter != nullptr; iter = iter->m_next) {
						if (iter->m_value == filter)
							break;
						else if (!in_cur_front[iter->m_value]) {
							in_cur_front[iter->m_value] = true;
							cur_candidate.push_back(iter->m_value);
						}
					}
				}
				// filter the cur_candidate
				for (int candidate : cur_candidate) {
					bool flag_in_cur_rank(true); // whether candidate is in current rank 
					for (const filter_sort::Node* iter = seq_by_obj_lists[min_idxs[candidate]].begin(); iter != nullptr; iter = iter->m_next) {
						if (iter->m_value == candidate)
							break;
						else {
							if (sol_stas[iter->m_value][max_idxs[iter->m_value]] > sol_stas[candidate][max_idxs[iter->m_value]])
								continue;
							// check whether solution[iter->m_value] donminate solution[candidate] or not
							bool flag_dominate(true);
							for (int i = 0; i < M; ++i)
								if (i != max_idxs[iter->m_value] && sol_stas[iter->m_value][i] > sol_stas[candidate][i]) {
									flag_dominate = false;
									break;
								}
							if (flag_dominate) {
								flag_in_cur_rank = false;
								break;
							}
						}
					}
					if (!flag_in_cur_rank)
						in_cur_front[candidate] = false;
				}
				// set rank for current Front
				std::vector<int> curFront;
				for (auto candidate : cur_candidate) {
					if (in_cur_front[candidate]) {
						rank[candidate] = cur_rank_number;
						num_not_ranked--;
						curFront.push_back(candidate);
					}
				}
				rank[filter] = cur_rank_number;
				num_not_ranked--;
				curFront.push_back(filter);
				// if terminate
				if (num_not_ranked == 0)
					break;
				// delete ranked solutions from lists
				for (auto candidate : curFront) {
					for (int i = 0; i < M; ++i)
						seq_by_obj_lists[i].erase(pos_in_obj_lists[candidate][i]);
					seq_by_sum_vals__lists.erase(pos_in_obj_lists[candidate][M]);
				}
				// goto next rank
				cur_rank_number++;
				cur_candidate.clear();
				memset(in_cur_front, false, N);
			}
			delete[] in_cur_front;
			return cur_rank_number;
		}

		template<typename T>
		int filterSortP(const std::vector<std::vector<T>*>& data, std::vector<int>& rank, const std::vector<OptimizeMode>& opt_mode, size_t num_task = -1) {
			if (num_task == -1)
				num_task = std::thread::hardware_concurrency();
			const int N = data.size(); // number of solution
			if (N == 0)	return -1;
			const int M = data[0]->size(); // number of obective
			if (rank.empty() || rank.size() != N)
				rank.resize(N);
			std::vector<std::vector<int>> seq_by_obj(M);

			int task_size = M;
			std::vector<std::thread> thrd;
			for (int i = 0; i < num_task; ++i) {
				std::vector<int> obj_idxs;
				for (int idx = i; idx < task_size; idx += num_task)
					obj_idxs.push_back(idx);
				thrd.push_back(std::thread(filter_sort::parallelMergeSort<T>, std::cref(data), std::ref(seq_by_obj), obj_idxs, std::cref(opt_mode)));
			}
			for (auto& t : thrd)
				t.join();

			std::vector<filter_sort::List> seq_by_obj_lists(M); // same as seq_by_obj but in form of LS_list
			std::vector<std::vector<filter_sort::Node*>> pos_in_obj_lists(N); // pos_in_obj_lists[i] stores solution[i]'s all LS_node addresses 
			for (int i = 0; i < M; ++i) {
				for (int sol_idx : seq_by_obj[i])
					pos_in_obj_lists[sol_idx].push_back(seq_by_obj_lists[i].push_back(sol_idx));
			}
			std::vector<std::vector<int>> sol_stas(N); // sol_stas[i] stores solution[i]'s all single objective sequence numbers
			for (int i = 0; i < M; ++i) {
				int seq_num = 0;
				for (filter_sort::Node* iter = seq_by_obj_lists[i].begin(); iter != nullptr; iter = iter->m_next) {
					sol_stas[iter->m_value].push_back(seq_num);
					seq_num++;
				}
			}

			std::vector<int> max_idxs(N); // max_idxs[i] means index of solution[i]'s maximum objective sequence number
			std::vector<int> min_idxs(N); // min_idxs[i] means index of solution[i]'s minimum objective sequence number
			std::vector<int> sum_vals(N); // sum_vals[i] means sum of solution[i]'s all objective sequence numbers
			for (int i = 0; i < N; ++i) {
				int max_val = -1;
				int min_val = N; // value of solution's minimum single objective sequence number
				for (int ObjIdx = 0; ObjIdx < M; ++ObjIdx) {
					if (sol_stas[i][ObjIdx] < min_val) {
						min_val = sol_stas[i][ObjIdx];
						min_idxs[i] = ObjIdx;
					}
					if (sol_stas[i][ObjIdx] > max_val) {
						max_val = sol_stas[i][ObjIdx];
						max_idxs[i] = ObjIdx;
					}
					sum_vals[i] += sol_stas[i][ObjIdx];
				}
			}

			std::vector<int> seq_by_sum_vals(N); // sequence of solution sorted by sum of all single objective sequence numbers
			mergeSort(sum_vals, N, seq_by_sum_vals);
			filter_sort::List seq_by_sum_vals_lists; // Same as SeqByMinVals but in form of LS_list
			for (const int sol_index : seq_by_sum_vals)
				pos_in_obj_lists[sol_index].push_back(seq_by_sum_vals_lists.push_back(sol_index));
			int cur_rank_number = 0; // current rank number
			int num_not_ranked = N; // number of solutions not ranked
			std::vector<int> cur_candidate; // the candidate solutions of current front
			cur_candidate.reserve(N);
			bool* in_cur_front = new bool[N]; // whether in current front
			memset(in_cur_front, false, N);
			int filter; // the solution chosen to generate CurRankCandidate
			while (num_not_ranked > 0) {
				// set filter
				filter = seq_by_sum_vals_lists.begin()->m_value;
				// generate cur_candidate by filter
				for (auto& seq_by_obj_list : seq_by_obj_lists) {
					for (auto iter = seq_by_obj_list.begin(); iter != nullptr; iter = iter->m_next) {
						if (iter->m_value == filter)
							break;
						else if (!in_cur_front[iter->m_value]) {
							in_cur_front[iter->m_value] = true;
							cur_candidate.push_back(iter->m_value);
						}
					}
				}
				// filter the cur_candidate
				task_size = cur_candidate.size();
				thrd.clear();
				for (int i = 0; i < num_task; ++i) {
					std::vector<int> candidates;
					for (int idx = i; idx < task_size; idx += num_task)
						candidates.push_back(cur_candidate[idx]);
					thrd.push_back(std::thread(filter_sort::parallelFilter, candidates, std::cref(seq_by_obj_lists), std::cref(max_idxs), std::cref(min_idxs), M, std::cref(sol_stas), in_cur_front));
				}
				for (auto& t : thrd)
					t.join();
				// set rank for current Front
				std::vector<int> cur_front;
				for (auto candidate : cur_candidate) {
					if (in_cur_front[candidate]) {
						rank[candidate] = cur_rank_number;
						num_not_ranked--;
						cur_front.push_back(candidate);
					}
				}
				rank[filter] = cur_rank_number;
				num_not_ranked--;
				cur_front.push_back(filter);
				// if terminate
				if (num_not_ranked == 0)
					break;
				// delete ranked solutions from lists
				for (auto candidate : cur_front) {
					for (int i = 0; i < M; ++i)
						seq_by_obj_lists[i].erase(pos_in_obj_lists[candidate][i]);
					seq_by_sum_vals_lists.erase(pos_in_obj_lists[candidate][M]);
				}
				// goto next rank
				cur_rank_number++;
				cur_candidate.clear();
				memset(in_cur_front, false, N);
			}
			delete[] in_cur_front;
            return cur_rank_number;
		}


	}
}
#endif // !OFEC_FILTER_SORT_H

