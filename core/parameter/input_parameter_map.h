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

#ifndef OFEC_INPUT_PARAMETER_MAP_H
#define OFEC_INPUT_PARAMETER_MAP_H

#include "parameter_map.h"
#include <map>
#include <list>
#include <memory>

namespace ofec {
	class InputParameter;

	class InputParameterMap {
	private:
		std::map<std::string, std::unique_ptr<InputParameter>> m_map;
		using IteratorType = std::map<std::string, std::unique_ptr<InputParameter>>::iterator;
		using CIteratorType = std::map<std::string, std::unique_ptr<InputParameter>>::const_iterator;
		using RIterType = std::map<std::string, std::unique_ptr<InputParameter>>::reverse_iterator;
		using RCIterType = std::map<std::string, std::unique_ptr<InputParameter>>::const_reverse_iterator;

	public:
		virtual ~InputParameterMap() = default;
		void add(const std::string &name, InputParameter *input_parameter);
		void erase(const std::string& name) {
			m_map.erase(name);
		}
		InputParameter* at(const std::string &name) const;
		void input(const ParameterMap &parameters) const;
		void input(const std::map<std::string, std::string> &parameters) const;
		void output(ParameterMap &parameters) const;
		void output(std::map<std::string, std::string> &parameters) const;
		void checkValue() const;

		size_t size() const { return m_map.size(); }
		CIteratorType begin() const { return m_map.begin(); }
		CIteratorType end() const { return m_map.end(); }
		CIteratorType cbegin() const { return m_map.cbegin(); }
		CIteratorType cend() const { return m_map.cend(); }
	};

	class InputParameter {
	protected:
		std::string m_requirement;
	public:
		virtual ~InputParameter() = default;
		const std::string& requirement() const { return m_requirement; }
		virtual void getVariantValue(ParameterVariant &value) const = 0;
		virtual void setVariantValue(const ParameterVariant &value) = 0;
		virtual void getValue(std::string &value) const = 0;
		virtual void setValue(const std::string &value) = 0;
		virtual void checkValue() const = 0;
	};

	template<typename TValue>
	class InputParameterValueType : public InputParameter {
	protected:
		TValue *m_value;
	public:
		InputParameterValueType(TValue &reference) : m_value(&reference) {}
		void getVariantValue(ParameterVariant &value) const override { value = *m_value; }
		void setVariantValue(const ParameterVariant &value) override { *m_value = std::get<TValue>(value); }
		void checkValue() const override {}
		const TValue& current() const { return *m_value; }
	};
	
	class InputParameterValueTypeBool : public InputParameterValueType<bool> {
	public:
		InputParameterValueTypeBool(bool &reference) : 
			InputParameterValueType(reference) {}
		void getValue(std::string &value) const override;
		void setValue(const std::string &value) override;
	};

	class InputParameterValueTypeInt : public InputParameterValueType<int> {
	public:
		InputParameterValueTypeInt(int &reference) : 
			InputParameterValueType(reference) {}
		void getValue(std::string &value) const override;
		void setValue(const std::string &value) override;
	};

	class InputParameterValueTypeSizeT : public InputParameterValueType<size_t> {
	public:
		InputParameterValueTypeSizeT(size_t &reference) : 
			InputParameterValueType(reference) {}
		void getValue(std::string &value) const override;
		void setValue(const std::string &value) override;
	};

	class InputParameterValueTypeChar : public InputParameterValueType<char> {
	public:
		InputParameterValueTypeChar(char &reference) : 
			InputParameterValueType(reference) {}
		void getValue(std::string &value) const override;
		void setValue(const std::string &value) override;
	};

	class InputParameterValueTypeReal : public InputParameterValueType<Real> {
	public:
		InputParameterValueTypeReal(Real &reference) : 
			InputParameterValueType(reference) {}
		void getValue(std::string &value) const override;
		void setValue(const std::string &value) override;
	};

	class InputParameterValueTypeVectorBool : public InputParameterValueType<std::vector<bool>> {
	public:
		InputParameterValueTypeVectorBool(std::vector<bool> &reference) :
			InputParameterValueType(reference) {}
		void getValue(std::string &value) const override;
		void setValue(const std::string &value) override;
	};

	class InputParameterValueTypeVectorInt : public InputParameterValueType<std::vector<int>> {
	public:
		InputParameterValueTypeVectorInt(std::vector<int> &reference) :
			InputParameterValueType(reference) {}
		void getValue(std::string &value) const override;
		void setValue(const std::string &value) override;
	};

	class InputParameterValueTypeVectorSizeT : public InputParameterValueType<std::vector<size_t>> {
	public:
		InputParameterValueTypeVectorSizeT(std::vector<size_t> &reference) :
			InputParameterValueType(reference) {}
		void getValue(std::string &value) const override;
		void setValue(const std::string &value) override;
	};

	class InputParameterValueTypeString : public InputParameterValueType<std::string> {
	public:
		InputParameterValueTypeString(std::string &reference) :
			InputParameterValueType(reference) {}
		void getValue(std::string &value) const override;
		void setValue(const std::string &value) override;
	};

	class InputParameterValueTypeVectorReal : public InputParameterValueType<std::vector<Real>> {
	public:
		InputParameterValueTypeVectorReal(std::vector<Real> &reference) :
			InputParameterValueType(reference) {}
		void getValue(std::string &value) const override;
		void setValue(const std::string &value) override;
	};

	class Bool : public InputParameterValueTypeBool {
	public:
		Bool(bool &reference, bool recommend);
	};

	class RangedInt : public InputParameterValueTypeInt {
	protected:
		int m_minimum, m_maximum;
	public:
		RangedInt(int &reference, int minimum, int maximum, int recommend);
		void checkValue() const override;
		int minimum() const { return m_minimum; }
		int maximum() const { return m_maximum; }
	};

	class EnumeratedInt : public InputParameterValueTypeInt {
	protected:
		std::list<int> m_enumeration;
	public:
		EnumeratedInt(int &reference, const std::list<int> &enumeration, int recommend);
		void checkValue() const override;
		const std::list<int> &enumeration() const { return m_enumeration; }
	};

	class RangedSizeT : public InputParameterValueTypeSizeT {
	protected:
		size_t m_minimum, m_maximum;
	public:
		RangedSizeT(size_t &reference, size_t minimum, size_t maximum, size_t recommend);
		void checkValue() const override;
		size_t minimum() const { return m_minimum; }
		size_t maximum() const { return m_maximum; }
	};

	class EnumeratedSizeT : public InputParameterValueTypeSizeT {
	protected:
		std::list<size_t> m_enumeration;
	public:
		EnumeratedSizeT(size_t &reference, const std::list<size_t> &enumeration, size_t recommend);
		void checkValue() const override;
		const std::list<size_t>& enumeration() const { return m_enumeration; }
	};

	class EnumerationBase : public InputParameter {
	protected:
		std::list<std::string> m_enumeration;
	public:
		EnumerationBase(const std::list<std::string> &enumeration);
		const std::list<std::string>& enumeration() const { return m_enumeration; }
		virtual size_t current() const = 0;
	};

	template<typename TEnum>
	class Enumeration : public EnumerationBase {
	protected:
		TEnum *m_enum_value;
	public:
		Enumeration(TEnum &reference, const std::list<std::string> &enumeration, TEnum recommend) :
			EnumerationBase(enumeration),
			m_enum_value(&reference)
		{
			*m_enum_value = recommend;
		}
		size_t current() const { return static_cast<size_t>(*m_enum_value); }
		void setVariantValue(const ParameterVariant &value) override {
			size_t idx = std::get<size_t>(value);
			if (idx < m_enumeration.size()) {
				*m_enum_value = static_cast<TEnum>(idx);
			}
			else {
				throw Exception("Invalid enumeration value.");
			}
		}
		void getVariantValue(ParameterVariant &value) const override {
			value = static_cast<size_t>(*m_enum_value);
		}
		void checkValue() const {
			if (static_cast<size_t>(*m_enum_value) >= m_enumeration.size()) {
				throw Exception(m_requirement);
			}
		}
		void setValue(const std::string &value) override {
			size_t idx = 0;
			for (auto &str : m_enumeration) {
				if (str == value) {
					break;
				}
				idx++;
			}
			if (idx < m_enumeration.size()) {
				*m_enum_value = static_cast<TEnum>(idx);
			}
			else {
				throw Exception("Invalid enumeration value.");
			}
		}
		void getValue(std::string &value) const override {
			size_t i = 0, idx = static_cast<size_t>(*m_enum_value);
			for (auto &str : m_enumeration) {
				if (i == idx) {
					value = str;
					break;
				}
				i++;
			}
		}
	};

	class RangedReal : public InputParameterValueTypeReal {
	protected:
		Real m_minimum, m_maximum;
	public:
		RangedReal(Real &reference, Real minimum, Real maximum, Real recommend);
		void checkValue() const override;
		Real minimum() const { return m_minimum; }
		Real maximum() const { return m_maximum; }
	};

	class EnumeratedReal : public InputParameterValueTypeReal {
	protected:
		std::list<Real> m_enumeration;
	public:
		EnumeratedReal(Real &reference, const std::list<Real> &enumeration, Real recommend);
		void checkValue() const override;
		const std::list<Real>& enumeration() const { return m_enumeration; }
	};

	class FileName : public InputParameterValueTypeString {
	protected:
		std::string m_directory_path, m_filter;
	public:
		FileName(std::string &reference, const std::string &directory_path, 
			const std::string &filter, const std::string &recommend);
		void checkValue() const override;
		const std::string& directoryPath() const { return m_directory_path; }
		const std::string& filter() const { return m_filter; }
	};

	class InputString : public InputParameterValueTypeString {
	public:
		InputString(std::string &reference, const std::string &recommend);
	};

	class DirectoryName : public InputParameterValueTypeString {
	
	public:
		DirectoryName(std::string& reference, const std::string& recommend);
		void checkValue() const override;
	};

	class EnumeratedString : public InputParameterValueTypeString {
	protected:
		std::list<std::string> m_enumeration;
	public:
		EnumeratedString(std::string &reference, const std::list<std::string> &enumeration,
			const std::string &recommend);
		void checkValue() const override;
		const std::list<std::string>& enumeration() const { return m_enumeration; }
	};
}
#endif // !OFEC_INPUT_PARAMETER_MAP_H
