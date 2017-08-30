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
* The vector-based objective encoding schme and a vector-based variable encoding scheme
* is defined.
*
*********************************************************************************/

#ifndef OFEC_ENCODING_H
#define OFEC_ENCODING_H

#include <vector>
#include <functional>

#include "../../utility/exception.h"
#include "../../utility/functional.h"

namespace OFEC {
	class base {
	public:
		virtual evaluation_tag evaluate(bool=true) = 0;
		virtual ~base() {};
		base(const base&) = default;
		base(base&&) = default;
		base() = default;
		base& operator=(const base&) = default;
		base& operator=(base&&) = default;
	};

	template<typename T = double>
	class objective {
	public:
		using value_type = T;
		using encoding = std::vector<value_type>;

		objective(size_t n=0) :m_o(n) {}
		objective(const objective& rhs) :m_o(rhs.m_o) {}
		objective(const std::vector<value_type>& rhs) :m_o(rhs) {}
		objective& operator=(const objective& rhs) {
#ifdef USING_EXCEPTION
			if (m_o.size() != rhs.m_o.size())
				throw exception("the number of objectives is not the same~");
#endif // USING_EXCEPTION
			if (this == &rhs)
				return *this;

			m_o = rhs.m_o;

			return *this;
		}
		objective& operator=(objective&& rhs) {
#ifdef USING_EXCEPTION
			if (m_o.size() != rhs.m_o.size())
				throw exception("the number of objectives is not the same~");
#endif // USING_EXCEPTION

			m_o = std::move(rhs.m_o);

			return *this;
		}

		objective(objective&& rhs) :m_o(std::move(rhs.m_o)) {}

		template<typename Compare = objective_compare<value_type>>
		dominationship compare(const std::vector<value_type>& rhs, const vector<optimization_mode> &mode, Compare comp = Compare()) const{
			return comp(m_o, rhs, mode);
		}

		template<typename Compare = objective_compare<value_type> >
		dominationship compare(const objective& rhs, const vector<optimization_mode> &mode, Compare comp = Compare()) const {
			return comp(m_o, rhs.m_o, mode);
		}

		template<typename Compare = objective_compare<value_type>>
		dominationship compare(const std::vector<value_type>& rhs, optimization_mode mode, Compare comp = Compare()) const {
			return comp(m_o, rhs, mode);
		}

		template<typename Compare = objective_compare<value_type> >
		dominationship compare(const objective& rhs, optimization_mode mode, Compare comp = Compare()) const {
			return comp(m_o, rhs.m_o, mode);
		}

		void resize(size_t n) {
			m_o.resize(n);
		}
		std::vector<value_type>& vect() {
			return m_o;
		}
		const std::vector<value_type>& vect() const{
			return m_o;
		}

		value_type& operator[](size_t n) {
			return m_o[n];
		}

		const value_type& operator[](size_t n)const {
			return m_o[n];
		}

		typename vector<value_type>::iterator begin() noexcept {
			return m_o.begin();
		}

		typename vector<value_type>::const_iterator begin() const noexcept {
			return m_o.cbegin();
		}

		typename vector<value_type>::iterator end() noexcept {
			return m_o.end();
		}

		typename vector<value_type>::const_iterator end() const noexcept {
			return m_o.cend();
		}

		int size() noexcept{
			return m_o.size();
		}

	protected:
		std::vector<value_type> m_o;
	
	};

	template <typename VariableType>
	class variable {
	public:
		using  value_type = typename VariableType;
		using encoding = std::vector<value_type>;

		variable(size_t n=0) :m_x(n) {}
		variable(const variable& rhs) : m_x(rhs.m_x) {}
		variable(variable&& rhs) :m_x(std::move(rhs.m_x)) {}
		variable(const std::vector<value_type>& x) : m_x(x) {}
		variable& operator=(const variable& rhs) {
#ifdef USING_EXCEPTION
			if (m_x.size() != rhs.m_x.size())
				throw exception("the number of dimensions is not the same!");
#endif // USING_EXCEPTION
			
			if (this == &rhs) return *this;

			m_x = rhs.m_x;

			return *this;
		}

		variable& operator=(variable&& rhs) {
#ifdef USING_EXCEPTION
			if (m_x.size() != rhs.m_x.size())
				throw exception("the number of dimensions is not the same!");
#endif // USING_EXCEPTION

			m_x = std::move(rhs.m_x);

			return *this;
		}

		void resize(size_t n) {
			m_x.resize(n);
		}
		int size() noexcept {
			return m_x.size();
		}
	protected:
		std::vector<value_type> m_x;
	};


	/*
		template <typename TypeVar, typename TypeObj = double>
	class variable :public objective<TypeObj> {
	public:
		using  value_type = typename TypeVar;
		//using 

		variable(size_t nd, size_t no) :objective(no) {}
		variable(const variable& rhs) :objective(rhs), m_x(rhs.m_x) {}
		variable(variable&& rhs) :objective(std::move(rhs)), m_x(std::move(rhs.m_x)) {}
		variable(const std::vector<value_type>& var, const std::vector<double>& obj) :objective(obj), m_x(var) {}
		variable& operator=(const variable& rhs) {
#ifdef USING_EXCEPTION
			if (m_x.size() != rhs.m_x.size())
				throw exception("the number of dimensions is not the same!");
#endif // USING_EXCEPTION
			
			if (this == &rhs) return *this;

			objective::operator=(rhs);
			m_x = rhs.m_x;

			return *this;
		}

		variable& operator=(variable&& rhs) {
#ifdef USING_EXCEPTION
			if (m_x.size() != rhs.m_x.size())
				throw exception("the number of dimensions is not the same!");
#endif // USING_EXCEPTION

			objective::operator=(std::move(rhs));
			m_x = std::move(rhs.m_x);

			return *this;
		}

	protected:
		std::vector<value_type> m_x;
	};
	*/
}

#endif // !OFEC_ENCODING_H

