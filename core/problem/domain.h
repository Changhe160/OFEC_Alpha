/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao & Junchen Wang & Changhe Li
* Email: diaoyiyacug@gmail & wangjunchen.chris@gmail.com & changhe.lw@gmail.com
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
* this file defines domain for vector-based variable with the same value type
*
*********************************************************************************/

#ifndef OFEC_DOMAIN_H
#define OFEC_DOMAIN_H

#include "../definition.h"
#include <utility>
#include <vector>

namespace ofec {
	template<typename TVal = Real>
	class Domain {
	public:
		struct SingleSet {
			bool limited = false;
			std::pair<TVal, TVal> limit = { std::numeric_limits<TVal>::lowest(), std::numeric_limits<TVal>::max() };
			TVal length = 0;
		};

		~Domain() = default;
		Domain(size_t n = 1) : m_range(n) {}
		void setRange(TVal low, TVal upper, size_t i) {
			m_range[i].limit.first = low;
			m_range[i].limit.second = upper;
			m_range[i].limited = true;
			m_range[i].length = upper - low;
			m_area_update = true;
			m_volume_update = true;
		}
		void resize(size_t n) {
			m_range.resize(n);
			m_area_update = true;
			m_volume_update = true;
		}
		const SingleSet& range(size_t i = 0) const {
			return m_range[i];
		}
		size_t size()const noexcept {
			return m_range.size();
		}
		const SingleSet& operator[](size_t i) const {
			return m_range[i];
		}
		SingleSet& operator[](size_t i) {
			return m_range[i];
		}
		void clear() {
			m_range.clear();
			m_domain_area = 0;
			m_domain_volume = 0;
			m_area_update = false;
			m_volume_update = false;
		}
		Real area() {
			if (m_area_update) {
				m_domain_area = 0;
				size_t limited_dim = 0;
				for (size_t j = 0; j < m_range.size(); ++j) {
					if (m_range[j].limited) {
						m_domain_area += (m_range[j].limit.second - m_range[j].limit.first) * (m_range[j].limit.second - m_range[j].limit.first);
						limited_dim++;
					}
				}
				m_domain_area = sqrt(m_domain_area / limited_dim);
				m_area_update = false;
			}
			return m_domain_area;
		}
		Real volume() {
			if (m_volume_update) {
				m_domain_volume = 1;
				for (size_t j = 0; j < m_range.size(); ++j) {
					m_domain_volume *= (m_range[j].limit.second - m_range[j].limit.first);
				}
				m_volume_update = false;
			}
			return m_domain_volume;
		}
		bool judgeInBoundary(std::vector<Real>& x) {
			bool feasible_flag = true;
			for (int idx(0); idx < m_range.size(); ++idx) {
				if (m_range[idx].limited) {
					if (x[idx] < m_range[idx].limit.first || x[idx] > m_range[idx].limit.second) {
						feasible_flag = false;
						break;
					}
				}
			}

			return feasible_flag;
		}

		bool isDomainLimited()const {
			for (auto& it : m_range) {
				if (it.limited)return true;
			}
			return false;
		}
	private:
		std::vector<SingleSet> m_range;
		bool m_area_update = false, m_volume_update = false;
		Real m_domain_area = 0;
		Real m_domain_volume = 0;
	};

}
#endif // !OFEC_DOMAIN_H