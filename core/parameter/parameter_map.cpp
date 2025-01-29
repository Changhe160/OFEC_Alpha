#include "parameter_map.h"

namespace ofec {
	bool operator==(const ParameterMap& p1, const ParameterMap& p2)
	{
		return p1.m_map == p2.m_map;
	}
	
	ParameterVariant& ParameterMap::operator[](const std::string &key) {
		return m_map[key];
	}

	const ParameterVariant& ParameterMap::at(const std::string &key) const {
		return m_map.at(key);
	}

	bool ParameterMap::has(const std::string &key) const {
		return m_map.count(key) > 0;
	}
	
	size_t ParameterMap::erase(const std::string &key) {
		return m_map.erase(key);
	}

	void ParameterMap::clear() {
		m_map.clear();
	}
}