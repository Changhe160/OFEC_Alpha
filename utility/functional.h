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
	struct euclidean_distance {
		double operator()(Iter1 first1, Iter1 last1, Iter2 first2) const {
			double dis = 0;
			while (first1 != last1) {
				dis += (*first1 - *first2)*(*first1 - *first2);
				++first1;
				++first2;
			}
			dis = sqrt(dis);
			return dis;
		}
	};


	template<typename Iter1, typename Iter2>
	struct manhattan_distance {
		double operator()(Iter1 first1, Iter1 last1, Iter2 first2) const{
			double dis = 0;
			while (first1 != last1) {
				dis += fabs(*first1++ - *first2++);
			}
			return dis;
		}
	};


	template<typename Iter1, typename Iter2>
	struct hamming_distance {
		int operator()(Iter1 first1, Iter1 last1, Iter2 first2) const {
			int dis = 0;
			while (first1 != last1) {
				dis += *first1++ != *first2++;
			}
			return dis;
		}
	};

	//domination relationship between two objective vectors
	template<typename T = double >
	struct objective_compare {
		dominationship operator()(const std::vector<T>& a, const std::vector<T>& b, const vector<optimization_mode> &mode) const {
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

		dominationship operator()(const std::vector<T>& a, const std::vector<T>& b, optimization_mode mode) const {
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
		using result_type = dominationship;
		using value_type = T;
	};

}
#endif // !OFEC_FINCTIONAL_H

