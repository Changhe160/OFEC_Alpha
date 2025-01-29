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

#ifndef OFEC_PARAMETER_VARIANT_H
#define OFEC_PARAMETER_VARIANT_H

#include <variant>
#include <vector>
#include <string>
#include <list>
#include <ostream>
#include "../definition.h"

namespace ofec {
	enum class ParameterType {
		kBool,
		kInt,
		kSizeT,
		kChar,
		kReal,
		kVectorBool,
		kVectorInt,
		kVectorSizeT,
		kString,
		kVectorReal,
		// to get the number of types in the ParameterVariant
		Count
	};

	using ParameterVariant = std::variant<
		bool,
		int,
		size_t,
		char,
		Real,
		std::vector<bool>,
		std::vector<int>,
		std::vector<size_t>,
		std::string,
		std::vector<Real>
	>;

	template<typename T, typename VARIANT_T>
	struct isVariantMember;

	// judge whether a type is one of ParameterVariant
	template<typename T, typename... ALL_T>
	struct isVariantMember<T, std::variant<ALL_T...>>
		: public std::disjunction<std::is_same<T, ALL_T>...> {};

	template<typename T>
	void getFrom(const ParameterVariant& v, T& value) {
		value = std::get<T>(v);
	}

	class ParameterVariantStream {
	public:
		using Type = std::pair<ParameterVariant, int>;

	protected:
		int m_last_id = 0;
		std::list<std::pair<ParameterVariant, int>> m_stream;

	public:

		virtual ~ParameterVariantStream() = default;
		size_t size()const {
			return m_stream.size();
		}

		std::list<std::pair<ParameterVariant, int>>& getStream() {
			return m_stream;
		}
		
		bool empty()const {
			return m_stream.empty();
		}
		
		void clear() {
			m_last_id = 0;
			m_stream.clear();
		}

		void pushVariant(ParameterVariant&& var) {
			m_stream.emplace_back(var, m_last_id++);
		}

		void pushVariant(ParameterVariant& var) {
			m_stream.emplace_back(var, m_last_id++);
		}

		std::pair<ParameterVariant, int> popVariant() {
			auto info = m_stream.front();
			m_stream.pop_front();
			--m_last_id;
			return std::move(info);
		}
		
		template<typename T>
		void input(const T& var) {
			m_stream.emplace_back(var, m_last_id++);
		}

		template<typename T> 
		void output(T& var) {
			auto info = m_stream.front();
			m_stream.pop_front();
			--m_last_id;
			getFrom(std::move(info.first), var);
		}
	};

	template<typename T>
	ParameterVariantStream& operator<< (
		ParameterVariantStream& stream, const T& value) {
		stream.input(value);
		return stream;
	}

	template<typename T>
	ParameterVariantStream& operator>> (
		ParameterVariantStream& stream, T& value) {
		stream.output(value);
		return stream;
	}
}

#endif
