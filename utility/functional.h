
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
#include <memory>

namespace OFEC {	
		/*
		distance measures between two std::vector-based points
		*/
	template<typename Iter1, typename Iter2>
	double euclidean_distance(Iter1 first1, Iter1 last1, Iter2 first2) {
		double dis = 0;
		while (first1 != last1) {
			dis += (*first1 - *first2)*(*first1 - *first2);
			++first1;
			++first2;
		}
		dis = sqrt(dis);
		return dis;
	}

	template<typename Iter1, typename Iter2>
	double manhattan_distance(Iter1 first1, Iter1 last1, Iter2 first2) {
		double dis = 0;
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
	template<typename T = double >
	std::pair<dominationship, int> objective_compare(const std::vector<T>& a, const std::vector<T>& b, const std::vector<optimization_mode> &mode)  {
		if (a.size() != b.size()) 
			return std::make_pair(dominationship::Non_comparable, 0);

		int comparisons = 0;
		int better = 0, worse = 0, equal = 0;
		for (decltype(a.size()) i = 0; i<a.size(); ++i) {
			comparisons++;
			if (mode[i] == optimization_mode::Minimization) {
				if (a[i] < b[i]) {
					if (worse > 0)
						return std::make_pair(dominationship::Non_dominated, comparisons);
					else
						++better;
				}
				else if (a[i] > b[i]) {
					if (better > 0)
						return std::make_pair(dominationship::Non_dominated, comparisons);
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
						return std::make_pair(dominationship::Non_dominated, comparisons);
					else
						++better;
				}
				else if (a[i] < b[i]) {
					if (better > 0)
						return std::make_pair(dominationship::Non_dominated, comparisons);
					else
						++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (equal == a.size()) return std::make_pair(dominationship::Equal, comparisons);
		else if (worse == 0) return std::make_pair(dominationship::Dominating, comparisons);
		else if (better == 0) return std::make_pair(dominationship::Dominated, comparisons);
	}

	template<typename T = double >
	std::pair<dominationship, int> objective_compare(const std::vector<T>& a, const std::vector<T>& b, optimization_mode mode) {
		int comparisons(0);
		if (a.size() != b.size())
			return std::make_pair(dominationship::Non_comparable, comparisons);

		int better = 0, worse = 0, equal = 0;
		for (decltype(a.size()) i = 0; i<a.size(); ++i) {
			comparisons++;
			if (mode == optimization_mode::Minimization) {
				if (a[i] < b[i]) {
					if (worse > 0)
						return std::make_pair(dominationship::Non_dominated, comparisons);
					else
						++better;
				}
				else if (a[i] > b[i]) {
					if (better > 0)
						return std::make_pair(dominationship::Non_dominated, comparisons);
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
						return std::make_pair(dominationship::Non_dominated, comparisons);
					else
						++better;
				}
				else if (a[i] < b[i]) {
					if (better > 0)
						return std::make_pair(dominationship::Non_dominated, comparisons);
					else
						++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (equal == a.size()) return std::make_pair(dominationship::Equal, comparisons);
		else if (worse == 0) return std::make_pair(dominationship::Dominating, comparisons);
		else if (better == 0) return std::make_pair(dominationship::Dominated, comparisons);
	}
	

	template <typename T>
	int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	template<class T>
	bool less(const T &d1, const T &d2, bool min = true) {
		if (min) {
			if (d1<d2) return true;
			else return false;
		}
		else {
			if (d1<d2) return false;
			else return true;
		}
	}

	template<class T>
	bool less(T *d1, T *d2, bool min = true) {
		if (min) {
			if (*d1<*d2) return true;
			else return false;
		}
		else {
			if (*d1<*d2) return false;
			else return true;
		}
	}

	template<class T>
	int quick_sort(const T &data, int size, std::vector<int>& index, bool min = true, int low = 0, int up = -1, int num = -1, bool start = true) {
		int num_comp(0);
		//sort data from small to large, and put the order in index
		//size: the size of data  
		//low, up : the range of data to be sorted
		//num : the max/mim number of data within low and up 
		static thread_local std::unique_ptr<int> lb;
		static thread_local std::unique_ptr<std::vector<bool>> flag;
		if (start)
		{
			if (up == -1) up = size - 1;
			if (num == -1) num = size;
			flag.reset(new std::vector<bool>(num, false));
			lb.reset(new int(low));
			if (index.size() == 0 || index.size() != size)		index.resize(size);
			for (auto i = index.begin(); i != index.end(); ++i) *i = i - index.begin();
		}


		if (low >= up) return num_comp;
		int i = 0;
		for (; i<num; i++) {
			if ((*flag.get())[i] == false)	break;
		}
		while (i < num) {
			if ((*flag.get())[i++] == false)	
				break;
		}
	
		if (i == num) return num_comp;
		int left = low + 1;
		int right = up;
		int pivot = low;

		while (left<right) {
			while (less(data[index[left]], data[index[pivot]], min) && left < right) {
				left++; 
				num_comp++;
			}
			num_comp++;
			while (!less(data[index[right]], data[index[pivot]], min) && left < right) { 
				right--;
				num_comp++; 
			}
			num_comp++;
			if (left<right) {
				int t = index[left];
				index[left] = index[right];
				index[right] = t;
			}
		}
		while (!less(data[index[left]], data[index[pivot]], min) && left > pivot) {
			left--;
			num_comp++;
		}
		num_comp++;
		num_comp++;
		if (less(data[index[left]], data[index[pivot]], min)) {
			int t = index[left];
			index[left] = index[pivot];
			index[pivot] = t;
			if (left - *lb<num)
				(*flag.get())[left - *lb] = true;
		}
		else
		{
			if (pivot - *lb<num)
				(*flag.get())[pivot - *lb] = true;
		}
		i = 0;
		for (; i<num; i++) {
			if ((*flag.get())[i] == false)		break;
		}
		if (i == num) return num_comp;

		pivot = left;
		num_comp += quick_sort(data, pivot - low, index, min, low, pivot - 1, num, false);
		num_comp += quick_sort(data, up - pivot, index, min, pivot + 1, up, num, false);
		return num_comp;
	}

}
#endif // !OFEC_FINCTIONAL_H

