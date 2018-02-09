#ifndef LINKSORT_H
#define LINKSORT_H
#include <vector>
#include "../../core/definition.h"
#include <chrono>

namespace NDS {
	struct LS_node
	{
		LS_node(const int value, LS_node* last = nullptr, LS_node* next = nullptr) : m_value(value), m_last(last), m_next(next) {}
		const int m_value;
		LS_node* m_last;
		LS_node* m_next;
};
	class LS_list
	{
	public:
		LS_list() : m_begin(nullptr), m_end(nullptr) {}
		LS_node* push_back(const int value) {
			LS_node* new_node(new LS_node(value));
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
		void erase(LS_node* node) {
			if (node != m_begin && node != m_end) {
				node->m_last->m_next = node->m_next;
				node->m_next->m_last = node->m_last;
			}
			else if (node == m_begin && node != m_end) {
				node->m_next->m_last = nullptr;
				m_begin = node->m_next;
			}
			else if (node == m_end && node != m_begin) {
				node->m_last->m_next = nullptr;
				m_end = node->m_last;
			}
			else {
				m_begin = nullptr;
				m_end = nullptr;
			}
			delete node;
		}
		LS_node* begin() { return m_begin; }
		LS_node* end() { return m_end; }
	private:
		LS_node* m_begin;
		LS_node* m_end;
	};
	void LinkSort(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement);
#ifdef USING_CONCURRENT
	void ParallelFilter(const std::vector<int>&& candidates, std::vector<LS_list>& SeqByObj_Lists, const std::vector<int>& MaxIdxs, const std::vector<int>& MinIdxs, const int N, const std::vector<std::vector<int>>& SolStas, bool* InCurRankCandiate);
	void ParallelQuickSort(const std::vector<std::vector<double>>& data, std::vector<std::vector<int>>& SeqByObj, const std::vector<int>&& ObjIdxs);
#endif // USING_CONCURRENT

}

#endif // !LINKSORT_H
