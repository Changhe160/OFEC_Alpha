/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Junchen Wang
* Email: changhe.lw@gmail.com, wangjunchen@cug.edu.cn
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 29 Mar 2018 by Junchen Wang
// Last modified:

#ifndef LINKSORT_H
#define LINKSORT_H
#include <vector>
#include "../../../core/definition.h"
#include <chrono>

namespace NS {
	struct fs_node
	{
		fs_node(const int value, fs_node* last = nullptr, fs_node* next = nullptr) : m_value(value), m_last(last), m_next(next) {}
		const int m_value;
		fs_node* m_last;
		fs_node* m_next;
};

	class fs_list
	{
	public:
		fs_list() : m_begin(nullptr), m_end(nullptr) {}
		~fs_list() {
			while (m_end != nullptr)
			{
				erase(m_end);
			}
		}
		fs_node* push_back(const int value) {
			fs_node* new_node(new fs_node(value));
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
		void erase(fs_node* node) {
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
		fs_node* begin() { return m_begin; }
		fs_node* end() { return m_end; }
		const fs_node* begin() const { return m_begin; }
		const fs_node* end() const { return m_end; }
	private:
		fs_node* m_begin;
		fs_node* m_end;
	};

	// Filter Sort Orignial Version
	int filter_sort(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement);
	// Filter Sort Modified Version 1 (without filter solution£©
	int filter_sort_m1(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement);
	// Filter Sort Modified Version 2 (without worst negate£©
	int filter_sort_m2(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement);
	// Filter Sort Parallel Computing Version
	int filter_sort_p(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement);
	// Output the number of non-dominated solutions of solutions in the first PF
	void output_num_cand(const std::vector<std::vector<double>>& data, const std::vector<int>& rank);
	// Function for Filter Sort Parallel Computing Version to check the validity of candidates
	void parallel_filter(const std::vector<int>&& candidates, const std::vector<fs_list>& SeqByObj_Lists, const std::vector<int>& MaxIdxs, const std::vector<int>& MinIdxs, const int N, const std::vector<std::vector<int>>& SolStas, bool* InCurFront);
	// Function for Filter Sort Parallel Computing Version to quick sort
	void parallel_quicksort(const std::vector<std::vector<double>>& data, std::vector<std::vector<int>>& SeqByObj, const std::vector<int>&& ObjIdxs);

}

#endif // !LINKSORT_H
