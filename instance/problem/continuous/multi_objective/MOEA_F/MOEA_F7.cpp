#include "MOEA_F7.h"

namespace OFEC {
	MOEA_F7::MOEA_F7(param_map & v) : problem(v[param_proName], v[param_numDim], 2), \
		MOEA_FBase(v[param_proName], v[param_numDim], 2) {
		v[param_numObj] = 2;
		m_dtype = 3;
		m_ptype = 21;
		m_ltype = 21;
		LoadPF();
	}
	MOEA_F7::MOEA_F7(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2) {
		m_dtype = 3;
		m_ptype = 21;
		m_ltype = 21;
		LoadPF();
	}
}