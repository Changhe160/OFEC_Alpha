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
*-------------------------------------------------------------------------------
* The vector-based objective_ encoding schme and a vector-based variable_ encoding scheme
* is defined.
*
*********************************************************************************/

#ifndef OFEC_ENCODING_H
#define OFEC_ENCODING_H

#include <vector>
#include "../definition.h"
#include <iostream>

namespace ofec {
	class VariableBase {
	public:
		virtual ~VariableBase() = default;
		virtual VariableBase& operator=(const VariableBase &rhs) = 0;
	};

	template <typename T = Real>
	class VariableVector : public VariableBase {
	public:
		using ValueType = T;
		using Encoding = std::vector<T>;
		using IteratorType = typename std::vector<T>::iterator;
		using CIteratorType = typename std::vector<T>::const_iterator;
		explicit VariableVector(size_t n = 0) : m_vector(n) {}
		VariableVector(size_t n, const T &val) : m_vector(n, val) {}
		VariableVector(const VariableVector &rhs) : m_vector(rhs.m_vector) {}
		VariableVector(VariableVector &&rhs) noexcept = default;
		VariableVector(const std::vector<T> &vars) : m_vector(vars) {}
		VariableVector& operator=(const VariableVector &rhs) {
			m_vector = rhs.m_vector;
			return *this;
		}
		VariableBase& operator=(const VariableBase &rhs) override {
			return VariableVector::operator=(dynamic_cast<const VariableVector&>(rhs));
		}
		VariableVector& operator=(VariableVector &&rhs) noexcept = default;
		VariableVector& operator=(const std::vector<T> &vars) {
			m_vector = vars;
			return *this;
		}
		friend std::ostream &operator<<(std::ostream &out, const VariableVector &vec) {
			if (vec.m_vector.empty())
				out << "{ }";
			else {
				out << "{ ";
				for (size_t i = 0; i < vec.m_vector.size() - 1; ++i)
					out << vec.m_vector[i] << ", ";
				out << vec.m_vector.back() << " }";
			}
			return out;
		}
		void resize(size_t n) { m_vector.resize(n); }
		size_t size() const noexcept { return m_vector.size(); }
		IteratorType begin() noexcept { return m_vector.begin(); }
		IteratorType end() noexcept { return m_vector.end(); }
		CIteratorType begin() const noexcept { return m_vector.begin(); }
		CIteratorType end() const noexcept { return m_vector.end(); }
		const T& operator[](size_t i) const { return m_vector[i]; }
		T& operator[](size_t i) { return m_vector[i]; }
		T* data() noexcept { return m_vector.data(); }
		const T* data() const noexcept { return m_vector.data(); }
		Encoding& vector() noexcept { return m_vector; }
		const Encoding& vector() const noexcept { return m_vector; }

	protected:
		std::vector<T> m_vector;
	};
}

#endif // !OFEC_ENCODING_H

