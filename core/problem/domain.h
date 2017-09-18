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
* this file defines domain for vector-based variable with the same value type
*
*********************************************************************************/

#ifndef OFEC_DOMAIN_H
#define OFEC_DOMAIN_H

#include "../algorithm/encoding.h"

namespace OFEC {

	template<typename ValueType>
	class domain {
	public:
		struct single_set {
			bool limited = true;
			std::pair<ValueType,ValueType> limit;
		};
		domain(int n=1):m_range(n){}
		void set_boundary_false(int i=0) {
			m_range[i].limited = false;
		}
		void set_range(ValueType low, ValueType upper, int i) {
			m_range[i].limit.first = low;
			m_range[i].limit.second = upper;
		}
		void resize(size_t n) {
			m_range.resize(n);
		}	
		const single_set& range(int i = 0)const {
			return m_range[i];
		}
		size_t size()const noexcept {
			return m_range.size();
		}
		const single_set& operator[](int i)const {
			 return m_range[i];
		}
		single_set& operator[](int i) {
			 return m_range[i];
		}
		
	private:
		std::vector<single_set> m_range;
	};

}
#endif // !OFEC_DOMAIN_H
