#include "MOEA_F6.h"

namespace OFEC {
	MOEA_F6::MOEA_F6(param_map & v) : problem(v[param_proName], v[param_numDim], 3), \
		MOEA_FBase(v[param_proName], v[param_numDim], 3) {
		v[param_numObj] = 3;
		std::vector<std::pair<real, real>> range(m_variable_size, std::make_pair(-2, 2));
		range[0].first = 0;
		range[0].second = 1;
		range[1].first = 0;
		range[1].second = 1;
		set_range(range);
		m_dtype = 1;
		m_ptype = 31;
		m_ltype = 32;
		LoadPF();
	}
	MOEA_F6::MOEA_F6(const std::string & name, size_t size_var) : problem(name, size_var, 3), \
		MOEA_FBase(name, size_var, 3) {
		std::vector<std::pair<real, real>> range(m_variable_size, std::make_pair(-2, 2));
		range[0].first = 0;
		range[0].second = 1;
		range[1].first = 0;
		range[1].second = 1;
		set_range(range);
		m_dtype = 1;
		m_ptype = 31;
		m_ltype = 32;
		LoadPF();
	}
}