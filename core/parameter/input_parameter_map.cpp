#include "input_parameter_map.h"
#include <filesystem>
#include "../../core/global.h"

namespace ofec {
	void InputParameterMap::add(const std::string &name, InputParameter *input_parameter) {
		if (m_map.count(name)) {
			throw Exception("The input parameter \"" + name + "\" has been duplicated.");
		}
		m_map.emplace(name, input_parameter);
	}

	InputParameter* InputParameterMap::at(const std::string &name) const {
		if (m_map.count(name)) {
			return m_map.at(name).get();
		}
		else {
			return nullptr;
		}
	}

	void InputParameterMap::input(const ParameterMap &parameters) const {
		for (auto &p : parameters) {
			if (m_map.count(p.first)) {
				m_map.at(p.first)->setVariantValue(p.second);
			}
		}
	}

	void InputParameterMap::input(const std::map<std::string, std::string> &parameters) const {
		for (auto &p : parameters) {
			if (m_map.count(p.first)) {
				m_map.at(p.first)->setValue(p.second);
			}
		}
	}

	void InputParameterMap::output(ParameterMap& parameters) const {
		for (auto &p : m_map) {
			p.second->getVariantValue(parameters[p.first]);
		}
	}

	void InputParameterMap::output(std::map<std::string, std::string> &parameters) const {
		for (auto &p : m_map) {
			p.second->getValue(parameters[p.first]);
		}
	}

	void InputParameterMap::checkValue() const {
		for (auto &p : m_map) {
			try {
				p.second->checkValue();
			}
			catch (const Exception &e) {
				throw Exception("The value of \"" + p.first + "\" is " + e.what() + '.');
			}
		}
	}

	void InputParameterValueTypeBool::getValue(std::string &value) const {
		value = *m_value ? "true" : "false";
	}

	void InputParameterValueTypeBool::setValue(const std::string &value) {
		*m_value = value[0] == 't' || value[0] == 'T';
	}

	void InputParameterValueTypeInt::getValue(std::string &value) const {
		value = std::to_string(*m_value);
	}

	void InputParameterValueTypeInt::setValue(const std::string &value) {
		*m_value = std::stoi(value);
	}

	void InputParameterValueTypeSizeT::getValue(std::string &value) const {
		value = std::to_string(*m_value);
	}

	void InputParameterValueTypeSizeT::setValue(const std::string &value) {
		*m_value = std::stoull(value);
	}

	void InputParameterValueTypeChar::getValue(std::string &value) const {
		value.resize(1);
		value[0] = *m_value;
	}

	void InputParameterValueTypeChar::setValue(const std::string &value) {
		*m_value = value[0];
	}

	void InputParameterValueTypeReal::getValue(std::string &value) const {
		value = std::to_string(*m_value);
	}

	void InputParameterValueTypeReal::setValue(const std::string &value) {
		*m_value = std::stod(value);
	}

	void InputParameterValueTypeVectorBool::getValue(std::string &value) const {
		// TODO
	}

	void InputParameterValueTypeVectorBool::setValue(const std::string &value) {
		// TODO
	}

	void InputParameterValueTypeVectorInt::getValue(std::string &value) const {
		// TODO
	}

	void InputParameterValueTypeVectorInt::setValue(const std::string &value) {
		// TODO
	}

	void InputParameterValueTypeVectorSizeT::getValue(std::string &value) const {
		// TODO
	}

	void InputParameterValueTypeVectorSizeT::setValue(const std::string &value) {
		// TODO
	}

	void InputParameterValueTypeString::getValue(std::string &value) const {
		value = *m_value;
	}

	void InputParameterValueTypeString::setValue(const std::string &value) {
		*m_value = value;
	}

	void InputParameterValueTypeVectorReal::getValue(std::string &value) const {
		// TODO
	}

	void InputParameterValueTypeVectorReal::setValue(const std::string &value) {
		// TODO
	}

	Bool::Bool(bool &reference, bool recommend) :
		InputParameterValueTypeBool(reference)
	{
		m_requirement = "a boolean value";
		*m_value = recommend;
	}

	RangedInt::RangedInt(int &reference, int minimum, int maximum, int recommend) :
		InputParameterValueTypeInt(reference),
		m_minimum(minimum),
		m_maximum(maximum) 
	{
		m_requirement = "an integer between "
			+ std::to_string(m_minimum) + " and " + std::to_string(m_maximum);
		*m_value = recommend;
	}

	void RangedInt::checkValue() const {
		if (*m_value < m_minimum || *m_value > m_maximum) {
			throw Exception(m_requirement);
		}
	}

	EnumeratedInt::EnumeratedInt(int &reference, const std::list<int> &enumeration, int recommend) :
		InputParameterValueTypeInt(reference),
		m_enumeration(enumeration)
	{
		m_requirement = "an integer from the enumeration { ";
		for (auto &e : m_enumeration) {
			m_requirement += std::to_string(e) + ", ";
		}
		m_requirement.pop_back();
		m_requirement += "}";
		*m_value = recommend;
	}

	void EnumeratedInt::checkValue() const {
		for (auto &e : m_enumeration) {
			if (*m_value == e) {
				return;
			}
		}
		throw Exception(m_requirement);
	}

	RangedSizeT::RangedSizeT(size_t &reference, size_t minimum, size_t maximum, size_t recommend) :
		InputParameterValueTypeSizeT(reference),
		m_minimum(minimum),
		m_maximum(maximum)
	{
		m_requirement = "an integer between "
			+ std::to_string(m_minimum) + " and " + std::to_string(m_maximum);
		*m_value = recommend;
	}

	void RangedSizeT::checkValue() const {
		if (*m_value < m_minimum || *m_value > m_maximum) {
			throw Exception(m_requirement);
		}
	}

	EnumeratedSizeT::EnumeratedSizeT(size_t &reference, const std::list<size_t> &enumeration, size_t recommend) :
		InputParameterValueTypeSizeT(reference),
		m_enumeration(enumeration)
	{
		m_requirement = "an integer from the enumeration { ";
		for (auto &e : m_enumeration) {
			m_requirement += std::to_string(e) + ", ";
		}
		m_requirement.pop_back();
		m_requirement += "}";
		*m_value = recommend;
	}

	void EnumeratedSizeT::checkValue() const {
		for (auto &e : m_enumeration) {
			if (*m_value == e) {
				return;
			}
		}
		throw Exception(m_requirement);
	}

	EnumerationBase::EnumerationBase(const std::list<std::string> &enumeration) :
		m_enumeration(enumeration)
	{
		m_requirement = "an item from the enumeration { ";
		size_t id = 0;
		for (auto &e : m_enumeration) {
			m_requirement += std::to_string(id++) + ": " + e + ", ";
		}
		m_requirement.pop_back();
		m_requirement += "}";
	}

	RangedReal::RangedReal(Real &reference, Real minimum, Real maximum, Real recommend) :
		InputParameterValueTypeReal(reference),
		m_minimum(minimum),
		m_maximum(maximum)
	{
		m_requirement = "a real number between "
			+ std::to_string(m_minimum) + " and " + std::to_string(m_maximum);
		*m_value = recommend;
	}

	void RangedReal::checkValue() const {
		if (*m_value < m_minimum || *m_value > m_maximum) {
			throw Exception(m_requirement);
		}
	}

	EnumeratedReal::EnumeratedReal(Real &reference, const std::list<Real> &enumeration, Real recommend) :
		InputParameterValueTypeReal(reference),
		m_enumeration(enumeration)
	{
		m_requirement = "a real number from the enumeration { ";
		for (auto &e : m_enumeration) {
			m_requirement += std::to_string(e) + ", ";
		}
		m_requirement.pop_back();
		m_requirement += "}";
		*m_value = recommend;
	}

	void EnumeratedReal::checkValue() const {
		for (auto &e : m_enumeration) {
			if (*m_value == e) {
				return;
			}
		}
		throw Exception(m_requirement);
	}

	FileName::FileName(std::string &reference, const std::string &directory_path,
		const std::string &filter, const std::string &recommend) :
		InputParameterValueTypeString(reference),
		m_directory_path(directory_path),
		m_filter(filter)
	{
		m_requirement = "a " + m_filter + " file in \"" + m_directory_path + '\"';
		*m_value = recommend;
	}

	void FileName::checkValue() const {
		
		//if (!std::filesystem::exists(g_working_directory + '/' + m_directory_path + '/' + *m_value)) {
		//	throw Exception(m_requirement);
		//}
	}

	InputString::InputString(std::string &reference, const std::string &recommend) :
		InputParameterValueTypeString(reference) 
	{
		*m_value = recommend;
	}

	DirectoryName::DirectoryName(std::string& reference, const std::string& recommend) :
		InputParameterValueTypeString(reference) 
	{
		*m_value = recommend;
	}

	void DirectoryName::checkValue() const {
		if (!std::filesystem::is_directory(*m_value)) {
			throw Exception(m_requirement);
		}
	}

	EnumeratedString::EnumeratedString(std::string &reference, 
		const std::list<std::string> &enumeration, const std::string &recommend) :
		InputParameterValueTypeString(reference),
		m_enumeration(enumeration)
	{
		m_requirement = "a string from the enumeration { ";
		for (auto &e : m_enumeration) {
			m_requirement += e + ", ";
		}
		m_requirement.pop_back();
		m_requirement += "}";
		*m_value = recommend;
	}

	void EnumeratedString::checkValue() const {
		for (auto &e : m_enumeration) {
			if (*m_value == e) {
				return;
			}
		}
		throw Exception(m_requirement);
	}
}