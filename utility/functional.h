
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
using namespace std;
namespace OFEC {
		
		/*
		distance measures between two vector-based points
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
	dominationship objective_compare(const std::vector<T>& a, const std::vector<T>& b, const vector<optimization_mode> &mode)  {
		if (a.size() != b.size()) 
			return dominationship::Non_comparable;

		int better = 0, worse = 0, equal = 0;
		for (decltype(a.size()) i = 0; i<a.size(); ++i) {
			if (mode[i] == optimization_mode::Minimization) {
				if (a[i]<b[i]) 
					++better;
				else if (a[i]>b[i]) 
					++worse;
				else 
					++equal;
			}
			else {
				if (a[i]>b[i]) {
					++better;
				}
				else if (a[i]<b[i]) {
					++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (better != 0 && better + equal == a.size()) return dominationship::Dominating;
		else if (worse != 0 && worse + equal == a.size()) return dominationship::Dominated;
		else if (equal == a.size()) return dominationship::Equal;
		else return dominationship::Non_dominated;
	}

	template<typename T = double >
	dominationship objective_compare(const std::vector<T>& a, const std::vector<T>& b, optimization_mode mode) {
		if (a.size() != b.size())
			return dominationship::Non_comparable;

		int better = 0, worse = 0, equal = 0;
		for (decltype(a.size()) i = 0; i<a.size(); ++i) {
			if (mode == optimization_mode::Minimization) {
				if (a[i]<b[i])
					++better;
				else if (a[i]>b[i])
					++worse;
				else
					++equal;
			}
			else {
				if (a[i]>b[i]) {
					++better;
				}
				else if (a[i]<b[i]) {
					++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (better != 0 && better + equal == a.size()) return dominationship::Dominating;
		else if (worse != 0 && worse + equal == a.size()) return dominationship::Dominated;
		else if (equal == a.size()) return dominationship::Equal;
		else return dominationship::Non_dominated;
	}
	

	template <typename T>
	int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

}
#endif // !OFEC_FINCTIONAL_H

