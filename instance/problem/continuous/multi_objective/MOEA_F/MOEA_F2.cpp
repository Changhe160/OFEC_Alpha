#include "MOEA_F2.h"

namespace OFEC {
	MOEA_F2::MOEA_F2(param_map & v) : problem(v[param_proName], v[param_numDim], 2), \
		MOEA_FBase(v[param_proName], v[param_numDim], 2) {
		v[param_numObj] = 2;
		std::vector<std::pair<real, real>> range(m_variable_size, std::make_pair(-1, 1));
		range[0].first = 0;
		set_range(range);
		m_dtype = 1;
		m_ptype = 21;
		m_ltype = 22;
		LoadPF();
	}
	MOEA_F2::MOEA_F2(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2) {
		std::vector<std::pair<real, real>> range(m_variable_size, std::make_pair(-1, 1));
		range[0].first = 0;
		set_range(range);
		m_dtype = 1;
		m_ptype = 21;
		m_ltype = 22;
		LoadPF();
	}
}