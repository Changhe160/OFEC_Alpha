/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao & Junchen Wang & Changhe Li
* Email: diaoyiyacug@gmail.com & wangjunchen.chris@gmail.com & changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*********************************************************************************/

#ifndef OFEC_PARAMETER_MAP_H
#define OFEC_PARAMETER_MAP_H

#include <utility>
#include <map>
#include "parameter_variant.h"
#include "../exception.h"
#include <ostream>

namespace ofec {
	class ParameterMap {
	private:
		std::map<std::string, ParameterVariant> m_map;
		using IteratorType = std::map<std::string, ParameterVariant>::iterator;
		using CIteratorType = std::map<std::string, ParameterVariant>::const_iterator;
		using RIterType = std::map<std::string, ParameterVariant>::reverse_iterator;
		using RCIterType = std::map<std::string, ParameterVariant>::const_reverse_iterator;

	public:

		virtual ~ParameterMap() = default;
		friend bool operator==(const ParameterMap& p1, const ParameterMap& p2);
		ParameterVariant& operator[](const std::string &key);
		const ParameterVariant& at(const std::string &key) const;
		bool has(const std::string &key) const;
		size_t erase(const std::string &key);
		void clear();
		CIteratorType begin() const { return m_map.begin(); }
		CIteratorType end() const { return m_map.end(); }
		CIteratorType cbegin() const { return m_map.cbegin(); }
		CIteratorType cend() const { return m_map.cend(); }

		template <typename T>
		const T& get(const std::string &key) const {
			if (m_map.count(key) > 0) {
				return std::get<T>(m_map.at(key));
			}
			else {
				throw Exception("parameter \"" + key + "\" is not given.");
			}
		}

		template <typename T>
		const T& get(const std::string& key, const T &default_val) const {
			if (m_map.count(key) > 0) {
				return std::get<T>(m_map.at(key));
			}
			else {
				return default_val;
			}
		}
	};


	template< typename T>
	inline void vecToParam(ParameterMap &v, const std::string &name, const std::vector<T> &vt) {
		v[name + char(2) + "size"] = int(vt.size());
		for (unsigned i(0); i < vt.size(); ++i) {
			v[name + char(2) + std::to_string(i)] = vt[i];
		}
	}

	template< typename T>
	inline std::vector<T> paramToVec(const ParameterMap &v, const std::string &name) {
		std::vector<T> vt(v.get<int>(name + char(2) + "size"));
		for (unsigned i(0); i < vt.size(); ++i) {
			vt[i] = v.get<T>(name + char(2) + std::to_string(i));
		}
		return vt;
	}

	template< typename T, typename K>
	void vecPairToParam(ParameterMap &v, const std::string &name, const std::vector<std::pair<T, K>> &vptt) {
		v[name + char(2) + "size"] = int(vptt.size());
		for (unsigned i(0); i < vptt.size(); ++i) {
			v[name + char(2) + std::to_string(i) + char(2) + "1"] = vptt[i].first;
			v[name + char(2) + std::to_string(i) + char(2) + "2"] = vptt[i].second;
		}
	}

	template< typename T, typename K>
	std::vector<std::pair<T, K>>  paramToVecPair(const ParameterMap &v, const std::string &name) {
		std::vector<std::pair<T, K>> vptt(v.get<int>(name + char(2) + "size"));
		for (unsigned i(0); i < vptt.size(); ++i) {
			vptt[i].first = v.get<T>(name + char(2) + std::to_string(i) + char(2) + "1");
			vptt[i].second = v.get<K>(name + char(2) + std::to_string(i) + char(2) + "2");
		}
		return vptt;
	}
}
#endif /* OFEC_PARAMETER_MAP_H */
