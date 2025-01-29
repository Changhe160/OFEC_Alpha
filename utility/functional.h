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
* some general methods used in ofec are defined in this file, including distance
* metrics, solution domination relationship, 
*
*********************************************************************************/

#ifndef OFEC_FUNCTIONAL_H
#define OFEC_FUNCTIONAL_H

#include <cmath>
#include <vector>
#include "../core/definition.h"
#include <cstdio>
#include <string>
#include <list>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <fstream>

#include "../core/problem/solution.h"
#include "../core/problem/problem.h"

namespace ofec {
	/*
	distance measures between two std::vector-based points
	*/
	template<typename Iter1, typename Iter2>
	inline Real euclideanDistance(Iter1 first1, Iter1 last1, Iter2 first2) {
		Real dis = 0;
		while (first1 != last1) {
			dis += (*first1 - *first2) * (*first1 - *first2);
			++first1;
			++first2;
		}
		dis = sqrt(dis);
		return dis;
	}


	template<typename Iter1>
	Real euclideanDistance(Iter1 first1, Iter1 last1) {
		Real dis = 0;
		while (first1 != last1) {
			dis += (*first1) * (*first1);
			++first1;
		}
		dis = sqrt(dis);
		return dis;
	}

	template<typename Iter1>
	Real manhattanDistance(Iter1 first1, Iter1 last1) {
		Real dis = 0;
		while (first1 != last1) {
			dis += fabs(*first1++);
		}
		return dis;
	}


	template<typename Iter1, typename Iter2>
	inline Real manhattanDistance(Iter1 first1, Iter1 last1, Iter2 first2) {
		Real dis = 0;
		while (first1 != last1) {
			dis += fabs(*first1++ - *first2++);
		}
		return dis;
	}

	template<typename Iter1, typename Iter2>
	inline int hammingDistance(Iter1 first1, Iter1 last1, Iter2 first2) {
		int dis = 0;
		while (first1 != last1) {
			dis += *first1++ != *first2++;
		}
		return dis;
	}

	template<typename Iter1, typename Iter2>
	inline Real pNormDistance(Iter1 first1, Iter1 last1, Iter2 first2, Real p) {
		Real dis = 0;
		while (first1 != last1) {
			dis += pow(abs(*first1++ - *first2++), p);
		}
		dis = pow(dis, 1 / p);
		return dis;
	}

	template<typename Iter1>
	inline Real pNormDistance(Iter1 first1, Iter1 last1, Real p) {
		Real dis = 0;
		while (first1 != last1) {
			dis += std::pow(abs(*first1++), p);
			++first1;
		}
		dis = pow(dis, 1 / p);
		return dis;
	}

	//Lp-norm (fractional distance, or Minkowski distance) for KDT-MOEA
	template<typename Iter1, typename Iter2>
	Real minkowski_distance(Iter1 first1, Iter1 last1, Iter2 first2, Real p = 0.5) {
		Real dis = 0;
		while (first1 != last1) {
			dis += pow(fabs(*first1++ - *first2++), p);
		}
		dis = pow(dis, 1.0 / p);
		return dis;
	}

	//domination relationship between two objective vectors
	template<typename T = Real >
	Dominance objectiveCompare(const std::vector<T>& a, const std::vector<T>& b, const std::vector<OptimizeMode>& mode) {
		if (a.size() < mode.size() || b.size() < mode.size())
			return Dominance::kNonComparable;

		int better = 0, worse = 0, equal = 0;
		for (size_t i = 0; i < mode.size(); ++i) {
			if (mode[i] == OptimizeMode::kMinimize) {
				if (a[i] < b[i]) {
					if (worse > 0)
						return Dominance::kNonDominated;
					else
						++better;
				}
				else if (a[i] > b[i]) {
					if (better > 0)
						return Dominance::kNonDominated;
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
						return Dominance::kNonDominated;
					else
						++better;
				}
				else if (a[i] < b[i]) {
					if (better > 0)
						return Dominance::kNonDominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (equal == mode.size()) return Dominance::kEqual;
		else if (worse == 0) return Dominance::kDominant;
		else if (better == 0) return Dominance::kDominated;
		else return Dominance::kNonDominated;
	}

	template<typename T = Real >
	Dominance objectiveCompare(const std::vector<T>& a, const std::vector<T>& b, OptimizeMode mode) {
		if (a.size() != b.size())
			return Dominance::kNonComparable;

		int better = 0, worse = 0, equal = 0;
		for (size_t i = 0; i < a.size(); ++i) {
			if (mode == OptimizeMode::kMinimize) {
				if (a[i] < b[i]) {
					if (worse > 0)
						return Dominance::kNonDominated;
					else
						++better;
				}
				else if (a[i] > b[i]) {
					if (better > 0)
						return Dominance::kNonDominated;
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
						return Dominance::kNonDominated;
					else
						++better;
				}
				else if (a[i] < b[i]) {
					if (better > 0)
						return Dominance::kNonDominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (equal == a.size()) return Dominance::kEqual;
		else if (worse == 0) return Dominance::kDominant;
		else if (better == 0) return Dominance::kDominated;
		else return Dominance::kNonDominated;
	}

	extern bool dominate(const SolutionBase &a, const SolutionBase &b, const std::vector<OptimizeMode> &opt_mode);

	//add by tanqingshan. return the first rank points.
	template <typename T = Real>
	std::vector<std::vector<T>*>& getNondominatedSet(std::vector<std::vector<T>*>& data, std::vector<OptimizeMode> opt_mode) {
		size_t n = data.size();
		std::vector<std::vector<T>*> obj = data;
		data.clear();
		data.push_back(obj[0]);
		for (size_t i = 1; i < n; i++) {
			size_t m = data.size();
			for (size_t j = 0; j < m; j++) {
				Dominance temp_ = objectiveCompare(*obj[i], *data[j], opt_mode);
				if (temp_ == Dominance::kEqual) {
					data.push_back(obj[i]);
				}
				else if (temp_ == Dominance::kDominant) {
					data[j] = obj[i];
					break;
				}
				else if (temp_ == Dominance::kNonDominated) {
					if (j == m - 1)
						data.push_back(obj[i]);
					else
						continue;
				}
				else {
					continue;
				}
			}
		}
		return data;
	}

	template <typename T = Real>
	std::vector<size_t> getNondominatedSetIndex(const std::vector<std::vector<T>>& data, std::vector<OptimizeMode> opt_mode) {
		size_t n = data.size();
		std::vector<std::vector<T>> obj = data;
		std::vector<size_t> front_inx;
		for (size_t i = 0; i < n; i++) {
			bool flag = true;
			for (size_t j = 0; j < n; j++) {
				if (j != i) {
					Dominance temp_ = objectiveCompare(data[i], data[j], opt_mode);
					if (temp_ == Dominance::kDominated) {
						flag = false;
						break;
					}
				}
			}
			if (flag) {
				front_inx.push_back(i);
			}
		}
		return front_inx;
	}

	template <typename T>
	int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	namespace merge_sort_func {
		template<typename T>
		bool less(const T& d1, const T& d2, bool ascending) {
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
		bool less(T* d1, T* d2, bool ascending) {
			if (ascending) {
				if (*d1 < *d2) return true;
				else return false;
			}
			else {
				if (*d1 < *d2) return false;
				else return true;
			}
		}

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
		void sort(const T& data, std::vector<int>& B, std::vector<int>& A, int l, int r, bool ascending) {
			if (r - l < 2)
				return;
			int m = (r + l) / 2;
			sort(data, A, B, l, m, ascending);
			sort(data, A, B, m, r, ascending);
			merge(data, B, A, l, m, r, ascending);
		}
	}

	/* index[i] denotes the index of the i-th minimum (in ascending order) element */
	template<typename T>
	void mergeSort(const T& data, int size, std::vector<int>& index, bool ascending = true, int low = 0, int up = -1, int num = -1, bool start = true) {
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
	std::vector<int> amend_order(const T& data, const std::vector<int>& index) {
		/*amend index in the case of the same item values in data. Note: data must be
		sorted with results stored in index, e.g.,
		data[]=[5,2,2,1]; index[]=[3,2,1,0]; //ascending order
		after amendation  index[]=[2,1,1,0];
		*/
		std::vector<int> result(index.size());
		for (int r = 1, idx = 1; r <= index.size(); r++, idx++) {
			int temp = r, count = 1;
			while (temp < index.size() && data[index[temp - 1]] == data[index[temp]]) {
				count++;
				temp++;
			}
			for (int k = 0; k < count; k++) result[index[r + k - 1]] = idx;
			r += count - 1;
		}
		return result;
	}

	template<typename T>
	inline T chaotic_value(T x, T min, T max, T rChaoticConstant = 3.54) {
		// return a value calculated by logistic map
		if (min > max) return -1;
		T val;
		val = (x - min) / (max - min);
		val = rChaoticConstant * val * (1 - val);
		return min + val * (max - min);
	}

	inline bool exists(const std::string& name) {
		if (FILE* file = fopen(name.c_str(), "r")) {
			fclose(file);
			return true;
		}
		else {
			return false;
		}
	}

	template<typename T>
	inline T mapReal(T value, T input_min, T input_max, T output_min, T output_max) {
		if (input_min == input_max)
			return (output_min + output_max) / 2.0;
		else
			return ((value - input_min) / (input_max - input_min) * (output_max - output_min) + output_min);
	}


	template<typename T>
	inline T mapRealInside(T value, T input_min, T input_max, T output_min, T output_max) {
		if (value < input_min) value = input_min;
		else if (value > input_max) value = input_max;
		return ((value - input_min) / (input_max - input_min) * (output_max - output_min) + output_min);
	}



	template<typename T, typename K>
	inline void calMeanAndStd(const std::vector<T> &data, K&mean, K&dev) {
		mean = 0;
		dev = 0;
		if (data.empty())return;

		for (T val : data)
			mean += val;
		mean /= static_cast<K>(data.size());

		for (T val : data)
			dev += (val - mean) * (val - mean);
		dev = sqrt(dev / static_cast<K>(data.size()));
	}

	template<typename T, typename K>
	inline void calMax(const std::vector<T>& data, K& maxValue) {
		if (data.empty()) {
			maxValue = 0;
			return;
		}
		maxValue = data.front();
		for (auto& it : data) {
			maxValue = std::max<T>(it, maxValue);
		}
	}

	template<typename T, typename K>
	inline void calMin(const std::vector<T>& data, K& maxValue) {
		if (data.empty()) {
			maxValue = 0;
			return;
		}
		maxValue = data.front();
		for (auto& it : data) {
			maxValue = std::min<T>(it, maxValue);
		}
	}

	template<typename T, typename K>
	inline void calMean(const std::vector<T>& data, K& mean) {
		mean = 0;
		for (T val : data)
			mean += val;
		mean /= static_cast<K>(data.size());
	}



	template< typename T>
	inline void calSum(const std::vector<T>& data, T& sum) {
		sum = 0;
		for (auto& it : data)sum += it;
	}



	template<typename T, typename K>
	inline void calStd(const std::vector<T>& data, const K& mean, K& std) {
		std = 0;
		for (T val : data)
			std += (val - mean) * (val - mean);
		std = sqrt(std / static_cast<K>(data.size()));
	}




	template<typename T>
	inline int calIdx(const std::vector<T>& inds, const std::function<bool(const T&, const T&)>& comp) {

		if (inds.empty()) return -1;
		int bestIdx(0);
		for (int curIdx(0); curIdx < inds.size(); ++curIdx) {
			if (comp(inds[curIdx], inds[bestIdx])) {
				bestIdx = curIdx;
			}
		}
		return bestIdx;
	}


	template<typename T>
	inline std::list<int> calBestIdx(const std::vector<T>& inds, const std::function<Dominance(const T&, const T&)>& comp) {

		if (inds.empty()) return std::list<int>();
		std::list<int> bestIdx;

		for (int idx(0); idx < inds.size(); ++idx) {
			bool flag = true;
			for (auto bestIter = bestIdx.begin(); bestIter != bestIdx.end(); ) {
				auto com_flag = comp(inds[idx], inds[*bestIter]);
				if (com_flag == Dominance::kDominated) {
					flag = false;
					break;
				}
				else if (com_flag == Dominance::kDominant) {
					bestIter = bestIdx.erase(bestIter);
				}
				else {
					bestIter++;
				}
			}
			if (flag) {
				bestIdx.push_back(idx);
			}
		}

		return bestIdx;

	}

	template<typename T, typename K>
	inline bool updateBestSol(std::vector<T>& inds, const K& newly_one, std::function<Dominance(const K&, const T&)>& comp) {
		std::vector<bool> flag(inds.size(), true);
		bool newly_one_flag = true;
		bool erase_flag = false;
		for (int idx(0); idx < inds.size(); ++idx) {
			auto com_flag = comp(newly_one, inds[idx]);
			if (com_flag == Dominance::kDominated) {
				newly_one_flag = false;
				break;
			}
			else if (com_flag == Dominance::kDominant) {
				flag[idx] = false;
				erase_flag = true;
			}
		}
		if (newly_one_flag) {
			if (erase_flag) {
				int cur_idx(0);
				for (int idx(0); idx < inds.size(); ++idx) {
					if (flag[idx]) {
						if (cur_idx != idx) {
							std::swap(inds[idx], inds[cur_idx]);
						}
						++cur_idx;
					}
				}
				inds.resize(cur_idx);
				return true;
			}
			else {
				return true;
			}
		}
		return false;
	}

	template <typename T>
	T least_square_estimation(const std::vector<T>& data) {
		T sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
		for (size_t i = 0; i < data.size(); ++i) {
			sum1 += ((i + 1) * data[i]);
			sum2 += (i + 1);
			sum3 += data[i];
			sum4 = sum4 + (i + 1) * (i + 1);
		}
		T a = (sum1 - sum2 * sum3 / data.size()) / (sum4 - sum2 * sum2 / data.size());
		T b = sum3 / data.size() - a * sum2 / data.size();

		return a * (data.size() + 1) + b;
	}

	template <typename T>
	Real vectorAngle(const std::vector<T>& ref_point, const std::vector<T>& point1, const std::vector<T>& point2) {
		size_t dim = ref_point.size();
		std::vector<T> vec1(dim, 0.), vec2(dim, 0.);
		for (int i = 0; i < dim; ++i) {
			vec1[i] = point1[i] - ref_point[i];
			vec2[i] = point2[i] - ref_point[i];
		}
		Real pro = 0.;
		Real norm1 = 0.;
		Real norm2 = 0.;
		for (int i = 0; i < dim; ++i) {
			pro += (vec1[i] * vec2[i]);
			norm1 += (vec1[i] * vec1[i]);
			norm2 += (vec2[i] * vec2[i]);
		}
		return std::acos(pro / std::sqrt(norm1 * norm2));
	}

	template <typename T>
	Real gammaFun(const T &n) {
		if (n == 0.5) {
			return std::sqrt(OFEC_PI);
		}
		else if (n == 1) {
			return 1;
		}
		else {
			return (n - 1) * gammaFun(n - 1);
		}
	}

	template <typename T>
	std::pair<Real, Real> vectorSpan(const std::vector<T>& ref_point, const std::vector<T>& point1, const std::vector<T>& point2) {
		std::pair<Real, Real> m_span;
		m_span.first = vectorAngle(ref_point, point1, point2);
		/*real length1 = euclidean_distance(ref_point.begin(),ref_point.end(),point1.begin());
		real length2 = euclidean_distance(ref_point.begin(), ref_point.end(), point2.begin());
		real average_length = (length1+length2) / 2;
		real span = m_span.first * average_length;*/
		Real span = euclideanDistance(point1.begin(), point1.end(), point2.begin());

		/*std::vector<real> c_point, temp_point1, temp_point2;
		if (point1[0] < point2[0]) {
			temp_point1 = point1;
			temp_point2 = point2;
		}
		else {
			temp_point1 = point2;
			temp_point2 = point1;
		}
		for (size_t i = 0; i < point1.size(); ++i) {
			if (i == 0)
				c_point.emplace_back(temp_point1[0]);
			else {
				c_point.emplace_back((temp_point1[0] - ref_point[0]) / (temp_point2[0] - ref_point[0])*(temp_point2[i] - ref_point[i]) + ref_point[i]);
			}
		}
		real temp_angle = vector_angle(temp_point1, c_point, temp_point2);
		real AB = euclidean_distance(temp_point1.begin(), temp_point1.end(), temp_point2.begin());
		span = span / (AB / 2 / (AB*sin(temp_angle) + AB * cos(temp_angle)) - 0.5);*/
		span = span / (1. / 2 + std::sqrt(2) / 4);
		size_t obj_num = ref_point.size();
		Real range = std::pow(OFEC_PI, (obj_num - 1.) / 2) / (obj_num - 1.) / gammaFun((obj_num - 1.) / 2 + 1) * std::pow(span / 2, obj_num - 1);
		m_span.second = range;
		return m_span;
	}

	template <typename T>
	std::pair<Real, Real> manifoldSpan(const std::vector<T>& ref_point, const std::vector<std::vector<T>>& points) {
		std::pair<Real, Real> m_span;
		// calculate the maximum span of point
		Real max_angle = -1. * 10e14;
		for (size_t i = 0; i < points.size(); ++i) {
			for (size_t j = i + 1; j < points.size(); ++j) {
				auto tmp_angle = vectorAngle(ref_point, points[i], points[j]);
				if (tmp_angle > max_angle)
					max_angle = tmp_angle;
				if (max_angle < 0) {
					size_t a = 1;
				}
			}
		}
		m_span.first = max_angle;
		//calculate the average radius
		Real sum_dist = 0.;
		for (size_t i = 0; i < points.size(); ++i) {
			auto length = euclideanDistance(ref_point.begin(), ref_point.end(), points[i].begin());
			sum_dist += length;
		}
		Real mean_length = sum_dist / points.size();

		size_t obj_num = ref_point.size();
		Real range = 2 * std::pow(OFEC_PI, obj_num / 2) * std::pow(mean_length, obj_num - 1) / gammaFun(obj_num / 2);
		m_span.second = range * max_angle;
		return m_span;
	}

	template<typename T>
	void dataNormalize(std::vector<T>& value) {
		T min_v = 1 * 10e14;
		T max_v = -1 * 10e14;
		for (auto& i : value) {
			if (i < min_v)
				min_v = i;
			if (i > max_v)
				max_v = i;
		}
		if (max_v != min_v) {
			for (size_t i = 0; i < value.size(); ++i) {
				value[i] = (value[i] - (Real)min_v) / (max_v - (Real)min_v);
			}
		}
		else {
			for (size_t i = 0; i < value.size(); ++i) {
				value[i] = 0.5;
			}
			//throw("data error!");
		}
	}


	template<typename T>
	void dataNormalizeInBound(std::vector<T>& value, const std::pair<T, T>& bound) {
		T min_v = bound.first;
		T max_v = bound.second;
		//for (auto& i : value) {
		//	if (i < min_v)
		//		min_v = i;
		//	if (i > max_v)
		//		max_v = i;
		//}
		if (max_v != min_v) {
			for (size_t i = 0; i < value.size(); ++i) {
				value[i] = (value[i] - (Real)min_v) / (max_v - (Real)min_v);
			}
		}
		else {
			for (size_t i = 0; i < value.size(); ++i) {
				value[i] = 0.5;
			}
			//throw("data error!");
		}
	}

	template<typename T>
	void dataNormalize(std::vector<std::vector<T>>& value) {
		if (value.empty()) {
			throw("no data!");
		}
		else {
			size_t num_data = value.size();
			size_t num_dim = value[0].size();
			for (size_t i = 0; i < num_dim; ++i) {
				T min_v = std::numeric_limits<T>::max();
				T max_v = std::numeric_limits<T>::min();
				for (size_t j = 0; j < num_data; ++j) {
					if (value[j][i] < min_v)
						min_v = value[j][i];
					if (value[j][i] > max_v)
						max_v = value[j][i];
				}
				if (max_v != min_v) {
					for (size_t j = 0; j < num_data; ++j) {
						value[j][i] = (value[j][i] - (Real)min_v) / (max_v - (Real)min_v);
					}
				}
				else {
					for (size_t j = 0; j < num_data; ++j) {
						value[j][i] = 0;
					}
				}
			}
		}
	}

	template<typename T>
	void dataNormalizeInBound(std::vector<std::vector<T>>& value,const std::vector<std::pair<T,T>> &bound) {
		if (value.empty()) {
			throw("no data!");
		}
		else {
			size_t num_data = value.size();
			size_t num_dim = value[0].size();
			for (size_t i = 0; i < num_dim; ++i) {
				for (size_t j = 0; j < num_data; ++j) {
					value[j][i] = (value[j][i] - bound[i].first) / (bound[i].second- bound[i].first);
				}
			}
		}
	}

	template<typename T>
	void dataRestore(std::vector<std::vector<T>>& value, std::vector<std::pair<T, T>>& boundary) {
		if (value.empty()) {
			throw("no data!");
		}
		else {
			size_t num_data = value.size();
			size_t num_dim = boundary.size();
			for (size_t i = 0; i < num_dim; ++i) {
				T min_v = boundary[i].first;
				T max_v = boundary[i].second;
				for (size_t j = 0; j < num_data; ++j) {
					value[j][i] = value[j][i] * (max_v - min_v) + min_v;
				}
			}
		}
	}

	//sort according to the value of the ind(th) column of the obj  
	template <typename T>
	void sortrows(std::vector<std::vector<T>>& obj, int ind) {
		if (!obj.empty()) {
			int row_num = obj.size();
			int col_num = obj[0].size();
			if (ind >= row_num) {

			}
			else {
				for (int i = 0; i < row_num - 1; i++) {
					for (int j = 0; j < row_num - i - 1; j++) {
						for (int k = 0; k < col_num; k++) {
							if (obj[j][k] > obj[j + 1][k]) {
								auto temp = obj[j + 1];
								obj[j + 1] = obj[j];
								obj[j] = temp;
								break;
							}
							else if (obj[j][k] < obj[j + 1][k]) {
								break;
							}
						}
					}
				}
				/*for (int i = 0; i < row_num - 1; i++) {
					for (int j = 0; j < row_num - i - 1; j++) {
						if (obj[j][ind] > obj[j + 1][ind]) {
							auto temp = obj[j + 1];
							obj[j + 1] = obj[j];
							obj[j] = temp;
						}
						else if (obj[i][ind] == obj[j][ind]) {
							for (int k = 1; k < col_num; k++) {
								if (obj[j][k] > obj[j + 1][k]) {
									auto temp = obj[j + 1];
									obj[j + 1] = obj[j];
									obj[j] = temp;
									break;
								}
								else if (obj[j][k] == obj[j + 1][k]) {
									continue;
								}
							}
						}
					}
				}*/
			}
		}
	}

	template <typename T>
	bool ifSame(const std::vector<T>& v1, const std::vector<T>& v2) {
		if (!v1.empty() && !v2.empty()) {
			if (v1.size() == v2.size()) {
				for (int i = 0; i < v1.size(); i++) {
					if (std::fabs(v1[i] - v2[i]) > 10e-9) {
						return false;
					}
				}
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}

	template <typename T>
	bool ifSame(const std::vector<std::vector<T>>& v1, const std::vector<std::vector<T>>& v2) {
		if (!v1.empty() && !v2.empty()) {
			int m1 = v1.size();
			int n1 = v1[0].size();
			int m2 = v2.size();
			int n2 = v2[0].size();
			if (m1 == m2 && n1 == n2) {
				for (int i = 0; i < m1; i++) {
					for (int j = 0; j < n1; j++) {
						if (std::fabs(v1[i][j] - v2[i][j]) > 10e-9) {
							return false;
						}
					}
				}
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}

	template<typename T>
	inline std::vector<T> range(T first, T last, T diff = 1) {
		std::vector<T> v((last - first) / diff + 1);
		for (size_t i = 0; i < v.size(); ++i)
			v[i] = first + diff * i;
		return v;
	}

	template<typename T>
	inline Real mean(const std::vector<T>& vec) {
		Real m = 0;
		for (size_t i = 0; i < vec.size(); ++i)
			m += vec[i];
		return m / vec.size();
	}

	template<typename T>
	inline std::vector<T> slice(const std::vector<T>& vec, const std::vector<int>& ids) {
		std::vector<T> v(ids.size());
		for (size_t i = 0; i < ids.size(); ++i)
			v[i] = ids[i] < 0 ? vec[ids.size() - ids[i]] : vec[ids[i]];
		return v;
	}

	inline Real normCDF(Real val) {
		return 0.5 * erfc(-val / sqrt(2));
	}

	template< typename T>
	std::vector<T> inputFileVec(std::ifstream& in) {
		unsigned size(0);
		in >> size;
		std::vector<T> vt(size);
		for (auto& it : vt) {
			in >> it;
		}
		return vt;
	}

	template< typename T>
	void outputFileVec(std::ofstream& out, const std::vector<T>& vt) {
		out << vt.size() << std::endl;
		for (auto& it : vt) {
			out << it << "\t";
		}
		out << std::endl;
	}

	template< typename T, typename K>
	std::vector<std::pair<T, K>> inputFileVecPair(std::ifstream& in) {
		unsigned size(0);
		in >> size;
		std::vector<std::pair<T, K>> vt(size);
		std::string tmp;
		for (auto& it : vt) {
			in >> tmp >> it.first >> tmp >> it.second >> tmp;
		}
		return vt;
	}

	template< typename T, typename K>
	void outputFileVecPair(std::ofstream& out, const std::vector<std::pair<T, K>>& vt) {
		out << vt.size() << std::endl;
		for (auto& it : vt) {
			out << " ( " << it.first << " , " << it.second << " ) " << "\t";
		}
		out << std::endl;
	}

	template <typename T>
	T maxElement(const std::vector<T>& obj) {
		T value = obj[0];
		for (size_t i = 0; i < obj.size(); ++i) {
			if (value < obj[i]) {
				value = obj[i];
			}
		}
		return value;
	}

	template <typename T>
	T minElement(const std::vector<T>& obj) {
		T value = obj[0];
		for (size_t i = 0; i < obj.size(); ++i) {
			if (value > obj[i]) {
				value = obj[i];
			}
		}
		return value;
	}

	template <typename T>
	std::vector<T> str2vector(std::string& ss, T v) {
		std::vector<T> output_vector;
		int i = 0, j = 0;
		while (j < ss.size()) {
			if (ss[j] == ',') {
				std::string str0 = ss.substr(i, j - i);
				i = j + 1;
				if (std::find(str0.begin(), str0.end(), '.') == str0.end()) {
					output_vector.push_back(std::stoi(str0));
				}
				else {
					output_vector.push_back(std::stof(str0));
				}
			}
			else if (j == ss.size() - 1) {
				std::string str0 = ss.substr(i, j + 1 - i);
				if (std::find(str0.begin(), str0.end(), '.') == str0.end()) {
					output_vector.push_back(std::stoi(str0));
				}
				else {
					output_vector.push_back(std::stof(str0));
				}
			}
			++j;
		}
		return output_vector;
	}

	// mesh grid 2-d space
	template <typename T>
	std::vector<std::vector<T>> meshGrid(std::vector<T>& x1, std::vector<T>& x2) {
		std::vector<std::vector<T>> output_vector;
		for (size_t i = 0; i < x1.size(); ++i) {
			std::vector<T> temp_sol(2, 0.);
			temp_sol[0] = x1[i];
			for (size_t j = 0; j < x2.size(); ++j) {
				temp_sol[1] = x2[j];
				output_vector.emplace_back(temp_sol);
			}
		}
		return output_vector;
	}

	template <typename T>
	std::vector<std::vector<T>> meshGrid(std::pair<T, T>& x1, std::pair<T, T>& x2, size_t num1, size_t num2) {
		//std::vector<std::vector<T>> output_vector;
		std::vector<T> xx1, xx2;
		for (size_t i = 0; i < num1; ++i) {
			xx1.push_back(x1.first + i * (x1.second - x1.first) / (num1 - 1));
		}
		for (size_t i = 0; i < num2; ++i) {
			xx2.push_back(x2.first + i * (x2.second - x2.first) / (num2 - 1));
		}
		return meshGrid(xx1, xx2);
	}

	// mesh grid 3-d space
	template<typename T>
	std::vector<std::vector<T>> mesh3dGrid(std::vector<T>& x1, std::vector<T>& x2, std::vector<T>& x3) {
		std::vector<std::vector<T>> m_2d_vector;
		for (size_t i = 0; i < x1.size(); ++i) {
			std::vector<T> temp_sol(2, 0.);
			temp_sol[0] = x1[i];
			for (size_t j = 0; j < x2.size(); ++j) {
				temp_sol[1] = x2[j];
				m_2d_vector.emplace_back(temp_sol);
			}
		}
		std::vector<std::vector<T>> output_vector;
		for (size_t i = 0; i < m_2d_vector.size(); ++i) {
			std::vector<T> temp = m_2d_vector[i];
			temp.push_back(0);
			for (size_t j = 0; j < x3.size(); ++j) {
				temp[2] = x3[j];
				output_vector.emplace_back(temp);
			}
		}
		return output_vector;
	}

	template <typename T>
	std::vector<std::vector<T>> meshGrid3d(std::pair<T, T>& x1, std::pair<T, T>& x2, std::pair<T, T>& x3, size_t num1, size_t num2, size_t num3) {
		//std::vector<std::vector<T>> output_vector;
		std::vector<T> xx1, xx2, xx3;
		for (size_t i = 0; i < num1; ++i) {
			xx1.push_back(x1.first + i * (x1.second - x1.first) / (num1 - 1));
		}
		for (size_t i = 0; i < num2; ++i) {
			xx2.push_back(x2.first + i * (x2.second - x2.first) / (num2 - 1));
		}
		for (size_t i = 0; i < num2; ++i) {
			xx3.push_back(x3.first + i * (x3.second - x3.first) / (num3 - 1));
		}
		return mesh3dGrid(xx1, xx2, xx3);
	}

	template <typename T>
	std::vector<std::vector<T>> meshGrid3d(std::vector<std::vector<T>>& x, std::vector<T>& x3) {
		std::vector<std::vector<T>> output_vector;
		for (size_t i = 0; i < x.size(); ++i) {
			std::vector<T> temp = x[i];
			temp.push_back(0);
			for (size_t j = 0; j < x3.size(); ++j) {
				temp[2] = x3[j];
				output_vector.emplace_back(temp);
			}
		}
		return output_vector;
	}

	template <typename T>
	std::vector<std::vector<T>> meshGrid3d(std::vector<std::vector<T>>& x, std::pair<T, T>& x3, size_t num) {
		std::vector<T> xx3;
		for (size_t i = 0; i < num; ++i) {
			xx3.push_back(x3.first + i * (x3.second - x3.first) / (num - 1));
		}
		return meshGrid3d(x, xx3);
	}

	template <typename T>
	std::vector<T> uniformSample(std::pair<T, T> x, size_t num) {
		std::vector<T> xx1;
		for (size_t i = 0; i < num; ++i) {
			xx1.push_back(x.first + i * (x.second - x.first) / (num - 1));
		}
		return xx1;
	}

	template <typename T>
	std::vector<std::vector<T>> distMatrix(std::vector<std::vector<T>>& points) {
		std::vector<std::vector<T>> dist_m;
		for (size_t i = 0; i < points.size(); ++i) {
			std::vector<T> dist;
			for (size_t j = 0; j < points.size(); ++j) {
				if (i == j) {
					dist.push_back(0.);
				}
				else {
					dist.push_back(euclideanDistance(points[i].begin(), points[i].end(), points[j].begin()));
				}
			}
			dist_m.emplace_back(dist);
		}
		return dist_m;
	}

	template <typename T>
	std::pair<std::pair<size_t, size_t>, T> matrixMinValue(const std::vector<std::vector<T>>& dist_matrix, bool flag) {
		auto temp_matrix = dist_matrix;
		std::pair<std::pair<size_t, size_t>, T> position_v;
		std::pair<size_t, size_t> position;
		if (flag) {// distance matrix
			for (size_t i = 0; i < temp_matrix.size(); ++i) {
				temp_matrix[i][i] += 10e10;

			}
		}
		std::vector<T> temp_min_dist;
		std::vector<size_t> temp_min_idx;
		for (size_t i = 0; i < temp_matrix.size(); ++i) {
			temp_min_dist.push_back(*std::min_element(temp_matrix[i].begin(), temp_matrix[i].end()));
			temp_min_idx.push_back(std::min_element(temp_matrix[i].begin(), temp_matrix[i].end()) - temp_matrix[i].begin());
		}
		position.first = std::min_element(temp_min_dist.begin(), temp_min_dist.end()) - temp_min_dist.begin();
		position.second = temp_min_dist[position.first];
		position_v.first = position;
		position_v.second = temp_matrix[position.first][position.second];
		return position_v;
	}

	template <typename T>
	std::vector<std::vector<T>> sampleIn2Points(std::vector<T>& point1, std::vector<T>& point2, size_t num) {
		auto s_p = point1;
		auto e_p = point2;
		std::vector<std::vector<Real>> output_p;
		for (size_t i = 0; i < num; ++i) {
			std::vector<Real> temp;
			for (size_t j = 0; j < s_p.size(); ++j) {
				temp.push_back(s_p[j] + i * (e_p[j] - s_p[j]) / (num - 1));
			}
			output_p.emplace_back(temp);
		}
		return output_p;
	}

	template <typename T>
	void sampleAmongPoints(std::vector<std::vector<T>>& points, std::vector<std::vector<T>>& output_points) {
		//first, calculate distance matrix among points
		auto distM=distMatrix(points);
		for (size_t i = 0; i <distM.size(); ++i) {
			for (size_t j = 0; j < distM.size(); ++j) {
				if (i == j) {
					distM[i][j] = (Real)INT16_MAX;
					break;
				}
			}
		}
		std::vector<Real> min_dist;
		for (size_t i = 0; i < distM.size(); ++i) {
			Real min_v = *std::min_element(distM[i].begin(), distM[i].end());
			min_dist.push_back(min_v);
		}
		Real all_min_dist = *std::min_element(min_dist.begin(), min_dist.end());
		auto it = points.begin();
		while (it != points.end()) {
			std::vector<Real> cur_p = *it;
			if (it == points.begin()) {
				output_points.emplace_back(cur_p);
			}
			else {
				std::vector<std::vector<Real>> temp_out;
				for (size_t p = 0; p < output_points.size();++p) {
					//deitermine number of samplings according to distance
					Real temp_dist = euclideanDistance(output_points[p].begin(), output_points[p].end(), (*it).begin());
					size_t sample_num = (size_t)std::ceil(temp_dist / all_min_dist) * 100;
					std::vector<std::vector<Real>> temp = sampleIn2Points(output_points[p], cur_p, sample_num);
					for (size_t i = 0; i < temp.size();++i) {
						temp_out.emplace_back(temp[i]);
					}
				}
				output_points.clear();
				output_points = temp_out;
			}
			++it;
		}
	}

	template <typename T>
	std::pair<size_t, size_t> crowdedPointPair(const std::vector<std::vector<T>>& points) {
		size_t m = points.size();
		Real min_dist = (Real)INT16_MAX;
		std::pair<size_t, size_t> out_pair;
		for (size_t i = 0; i < m; ++i) {
			std::vector<Real> temp;
			for (size_t j = 0; j < m; ++j) {
				if (i == j) {
					temp.push_back((Real)INT16_MAX);
				}
				else {
					temp.push_back(euclideanDistance(points[i].begin(),points[i].end(),points[j].begin()));
				}
			}
			Real min_v = *std::min_element(temp.begin(), temp.end());
			if (min_v < min_dist) {
				out_pair.first = i;
				out_pair.second = std::distance(temp.begin(), std::min_element(temp.begin(), temp.end()));
			}
		}
		return out_pair;
	}

	template <typename T>
	size_t selectFarthestPoint(const std::vector<std::vector<T>>& selected_points, const std::vector<std::vector<T>>& points) {
		size_t m = points.size();
		std::vector<Real> min_dist;
		for (size_t i = 0; i < m; ++i) {
			std::vector<Real> temp_dist;
			for (size_t j = 0; j < selected_points.size(); ++j) {
				temp_dist.push_back(euclideanDistance(points[i].begin(), points[i].end(), selected_points[j].begin()));
			}
			Real min_v = *std::min_element(temp_dist.begin(), temp_dist.end());
			min_dist.push_back(min_v);
		}

		return std::distance(min_dist.begin(),std::max_element(min_dist.begin(),min_dist.end()));
	}



}
#endif // !OFEC_FUNCTIONAL_H