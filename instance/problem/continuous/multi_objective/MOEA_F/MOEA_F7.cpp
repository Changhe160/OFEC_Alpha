#include "MOEA_F7.h"

namespace OFEC {
	MOEA_F7::MOEA_F7(param_map & v) : problem(v[param_proName], v[param_numDim], 2), \
		MOEA_FBase(v[param_proName], v[param_numDim], 2) {
		global::ms_arg[param_numObj] = 2;
		m_dtype = 3;
		m_ptype = 21;
		m_ltype = 21;
		LoadPF();
	}
}