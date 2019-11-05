
/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* some general methods used in OFEC are defined in this file, including distance
* metrics, solution domination relationship, 
*
*********************************************************************************/

#ifndef OFEC_FINCTIONAL_H
#define OFEC_FINCTIONAL_H

#include <cmath>
#include <vector>
#include "../core/definition.h"
#include <cstdio>
#include <string>

namespace OFEC {	
		/*
		distance measures between two std::vector-based points
		*/
	template<typename Iter1, typename Iter2>
	real euclidean_distance(Iter1 first1, Iter1 last1, Iter2 first2) {
		real dis = 0;
		while (first1 != last1) {
			dis += (*first1 - *first2)*(*first1 - *first2);
			++first1;
			++first2;
		}
		dis = sqrt(dis);
		return dis;
	}

	template<typename Iter1, typename Iter2>
	real manhattan_distance(Iter1 first1, Iter1 last1, Iter2 first2) {
		real dis = 0;
		while (first1 != last1) {
			dis += fabs(*first1++ - *first2++);
		}
		return dis;
	}
	


	template<typename Iter1, typename Iter2>
	int hamming_distance(Iter1 first1, Iter1 last1, Iter2 first2)  {
		int dis = 0;
		while (first1 != last1) {
			dis += *first1++ != *first2++;
		}
		return dis;
	}


	//domination relationship between two objective vectors
	template<typename T = real >
	dominationship objective_compare(const std::vector<T>& a, const std::vector<T>& b, const std::vector<optimization_mode> &mode)  {
		if (a.size() != b.size()) 
			return dominationship::Non_comparable;

		int better = 0, worse = 0, equal = 0;
		for (decltype(a.size()) i = 0; i<a.size(); ++i) {
			if (mode[i] == optimization_mode::Minimization) {
				if (a[i] < b[i]) {
					if (worse > 0)
						return dominationship::Non_dominated;
					else
						++better;
				}
				else if (a[i] > b[i]) {
					if (better > 0)
						return dominationship::Non_dominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
			else {
				if (a[i] > b[i]) {
					if (worse > 0)
						return dominationship::Non_dominated;
					else
						++better;
				}
				else if (a[i] < b[i]) {
					if (better > 0)
						return dominationship::Non_dominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (equal == a.size()) return dominationship::Equal;
		else if (worse == 0) return dominationship::Dominating;
		else if (better == 0) return dominationship::Dominated;
	}

	template<typename T = real >
	dominationship objective_compare(const std::vector<T> & a, const std::vector<T> & b, optimization_mode mode) {
		if (a.size() != b.size())
			return dominationship::Non_comparable;

		int better = 0, worse = 0, equal = 0;
		for (decltype(a.size()) i = 0; i < a.size(); ++i) {
			if (mode == optimization_mode::Minimization) {
				if (a[i] < b[i]) {
					if (worse > 0)
						return dominationship::Non_dominated;
					else
						++better;
				}
				else if (a[i] > b[i]) {
					if (better > 0)
						return dominationship::Non_dominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
			else {
				if (a[i] > b[i]) {
					if (worse > 0)
						return dominationship::Non_dominated;
					else
						++better;
				}
				else if (a[i] < b[i]) {
					if (better > 0)
						return dominationship::Non_dominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (equal == a.size()) return dominationship::Equal;
		else if (worse == 0) return dominationship::Dominating;
		else if (better == 0) return dominationship::Dominated;
	}

	template <typename T>
	int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	template<typename T>
	bool less(const T &d1, const T &d2, bool ascending) {
		if (ascending) {
			if (d1 < d2) return true;
			else return false;
		}
		else {
			if (d1 < d2) return false;
			else return true;
		}
	}

	template<typename T>
	bool less(T *d1, T *d2, bool ascending) {
		if (ascending) {
			if (*d1 < *d2) return true;
			else return false;
		}
		else {
			if (*d1 < *d2) return false;
			else return true;
		}
	}

	namespace merge_sort_func {
		template<typename T>
		void merge(const T& data, std::vector<int>& A, std::vector<int>& B, int l, int m, int r, bool ascending) {
			int i = l, j = m;
			for (int k = l; k < r; ++k) {
				if (i < m && (j >= r || less(data[A[i]], data[A[j]], ascending))) {
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
		void sort(const T& data, std::vector<int>& B, std::vector<int>& A, int l, int r, bool ascending){
			if (r - l < 2)
				return;
			int m = (r + l) / 2;
			sort(data, A, B, l, m, ascending);
			sort(data, A, B, m, r, ascending);
			merge(data, B, A, l, m, r, ascending);
		}
	}

	template<typename T>
	void merge_sort(const T &data, int size, std::vector<int>& index, bool ascending = true, int low = 0, int up = -1, int num = -1, bool start = true) {
		if (start) {
			if (up == -1) up = size - 1;
			if (num == -1) num = size;
			if (index.size() == 0 || index.size() != size)		index.resize(size);
			for (auto i = index.begin(); i != index.end(); ++i) *i = i - index.begin();
		}
		if (low >= up) return;

		std::vector<int> B(index);
		merge_sort_func::sort(data, B, index, low, up + 1, ascending);
	}

	template <class T>
	std::vector<int> amend_order(const T &data, const std::vector<int> &index) {
		/*amend index in the case of the same item values in data. Note: data must be
		sorted with results stored in index, e.g.,
		data[]=[5,2,2,1]; index[]=[3,2,1,0]; //ascending order
		after amendation  index[]=[2,1,1,0];
		*/
		std::vector<int> result(index.size());
		for (int r = 1, idx = 1; r <= index.size(); r++, idx++) {
			int temp = r, count = 1;
			while (temp<index.size() &&data[index[temp - 1]] == data[index[temp]]) {
				count++; 
				temp++; 
			}
			for (int k = 0; k<count; k++) result[index[r + k - 1]] = idx;
			r += count - 1;
		}
		return result;
	}

	inline real chaotic_value(real x, real min, real max, real rChaoticConstant = 3.54) {
		// return a value calculated by logistic map
		if (min>max) return -1;
		real val;
		val = (x - min) / (max - min);
		val = rChaoticConstant*val*(1 - val);
		return min + val*(max - min);
	}

	inline bool exists(const std::string& name) {
		if (FILE *file = fopen(name.c_str(), "r")) {
			fclose(file);
			return true;
		}
		else {
			return false;
		}
	}

	inline real map_real(real value, real input_min, real input_max, real output_min, real output_max) {
		return ((value - input_min) / (input_max - input_min) * (output_max - output_min) + output_min);
	}
}
#endif // !OFEC_FINCTIONAL_H

